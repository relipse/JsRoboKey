#include "dlgjsrobokey.h"
#include "ui_dlgjsrobokey.h"

#include <QTextStream>
#include <QFile>
#include <QtNetwork>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QUrl>

#include "jsrobokey.h"

DlgJsRoboKey::DlgJsRoboKey(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgJsRoboKey),
    m_pjsrobokey(NULL), m_jsengine(NULL)
{
    ui->setupUi(this);

    m_pjsrobokey = new JsRoboKey(this);

    JSEdit& editor = *ui->memoInstaScript;
    editor.setWindowTitle("Insta Run");
    editor.setFrameShape(JSEdit::NoFrame);
    editor.setWordWrapMode(QTextOption::NoWrap);
    editor.setTabStopWidth(4);
    //editor.resize(QApplication::desktop()->availableGeometry().size() / 2);
    QStringList keywords = editor.keywords();
    keywords << "const";
    keywords << "let";
    editor.setKeywords(keywords);

    // dark color scheme
    editor.setColor(JSEdit::Background,    QColor("#0C152B"));
    editor.setColor(JSEdit::Normal,        QColor("#FFFFFF"));
    editor.setColor(JSEdit::Comment,       QColor("#666666"));
    editor.setColor(JSEdit::Number,        QColor("#DBF76C"));
    editor.setColor(JSEdit::String,        QColor("#5ED363"));
    editor.setColor(JSEdit::Operator,      QColor("#FF7729"));
    editor.setColor(JSEdit::Identifier,    QColor("#FFFFFF"));
    editor.setColor(JSEdit::Keyword,       QColor("#FDE15D"));
    editor.setColor(JSEdit::BuiltIn,       QColor("#9CB6D4"));
    editor.setColor(JSEdit::Cursor,        QColor("#1E346B"));
    editor.setColor(JSEdit::Marker,        QColor("#DBF76C"));
    editor.setColor(JSEdit::BracketMatch,  QColor("#1AB0A6"));
    editor.setColor(JSEdit::BracketError,  QColor("#A82224"));
    editor.setColor(JSEdit::FoldIndicator, QColor("#555555"));

    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    setIcon(1);
    trayIcon->show();


    //initalize the v8 engine
    initialize();
}

void  DlgJsRoboKey::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}


void DlgJsRoboKey::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void DlgJsRoboKey::showMessage(const QString& title, const QString& body, int iicon, int ms_duration, const QJSValue& callback,
                               const QString& action, const QString& param1)
 {
    //actions are to be defined in nature
    //TODO: notice, this will screw up the previous callback, does it matter?
    trayJsCallback = callback;
    trayAction = action;
    trayParam1 = param1;
    // NoIcon, Information, Warning, Critical
    QSystemTrayIcon::MessageIcon icon = (QSystemTrayIcon::MessageIcon)iicon;
    trayIcon->showMessage(title, body, icon, ms_duration);
 }


void DlgJsRoboKey::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void DlgJsRoboKey::closeEvent(QCloseEvent *event)
 {
     if (trayIcon->isVisible()) {
         /*
         QMessageBox::information(this, tr("Systray"),
                                  tr("The program will keep running in the "
                                     "system tray. To terminate the program, "
                                     "choose <b>Quit</b> in the context menu "
                                     "of the system tray entry."));*/
         showMessage("JsRoboKey is still running in the tray",
                     "To exit the program completly and all running scripts, Right click -> quit ", 1);
         hide();
         event->ignore();
     }
 }

void DlgJsRoboKey::messageClicked()
 {
     //TODO: depending on the action we need to do something
    if (trayJsCallback.isCallable()){
        trayJsCallback.call();
        //it was already called, now make it undefined
        trayJsCallback = QJSValue();
    }
 }

void DlgJsRoboKey::iconActivated(QSystemTrayIcon::ActivationReason reason)
 {
     switch (reason) {
     case QSystemTrayIcon::Trigger:
     case QSystemTrayIcon::DoubleClick:
         //do something
         setVisible(true);
         break;
     case QSystemTrayIcon::MiddleClick:
         //what the heck are we going to do with a middle click?
         break;
     default:
         ;
     }
 }

void DlgJsRoboKey::setIcon(int index)
{

    QIcon icon = QIcon(tr(":/jsrobokeyicon%1.png").arg(index));
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}

DlgJsRoboKey::~DlgJsRoboKey()
{ 
    delete m_jsengine;
    delete m_pjsrobokey;
    delete ui;
}

/**
 * @brief DlgJsRoboKey::initialize
 *  Initialize all global objects and variables
 */
void DlgJsRoboKey::initialize()
{
    delete m_jsengine;
    m_jsengine = new QJSEngine(this);
    m_rk = m_jsengine->newQObject(m_pjsrobokey);
    m_jsengine->globalObject().setProperty("JsRoboKey", m_rk);

    //allow rk for short
    //make some functions global scope
    loadJS("jsrk = rk = JsRoboKey; "
           "alert = rk.alert; "
           "include = rk.include; "
           "require = rk.require"
           "__FILE__ = 'JsRoboKey::initialize()'; ", "JsRoboKey::initialize()");
}


bool DlgJsRoboKey::loadJSFile(const QString &file)
{
    QFile scriptFile(file);
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    // handle error
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    return loadJS(contents, file);
}

bool DlgJsRoboKey::loadJS(const QString &code, const QString& module_or_filename)
{
    m_lastException = tr("");
    m_lastRunCode = code;
    m_lastRunFileOrModule = module_or_filename;
    m_lastRunVal = m_jsengine->evaluate(code, module_or_filename);
    if (m_lastRunVal.isError())
    {
        m_lastException = m_lastRunVal.toString();
        return false;
    }
    return true;
}



void DlgJsRoboKey::on_btnInstaRun_clicked()
{
    loadJS(ui->memoInstaScript->toPlainText(), "instarun");
    ui->lblStatus->setText(m_lastRunVal.toString());
}


QJSEngine *DlgJsRoboKey::jsengine()
{
   return m_jsengine;
}


void DlgJsRoboKey::on_btnUnloadAll_clicked()
{
    initialize();
}
