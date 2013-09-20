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
    QDialog(parent, Qt::Window | Qt::WindowSystemMenuHint
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowMaximizeButtonHint
            | Qt::WindowCloseButtonHint),
    ui(new Ui::DlgJsRoboKey),
    m_pjsrobokey(NULL), m_jsengine(NULL), m_mainScriptLoaded(false)
{
    ui->setupUi(this);
    ui->memoInstaScript->setSuppressCtrlEnter(true);
    connect(ui->memoInstaScript, SIGNAL(onCtrlEnter()), this, SLOT(on_btnInstaRun_clicked()));

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

    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(on_trayMessageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    setIcon(1);
    trayIcon->show();


    //initalize the v8 engine
    initialize();

    //Currently the tray icon ALWAYS loads, this is for safety, later on do we allow turning it off via command line argument?
    bool loadMainScriptFile = true;
    QStringList args = QApplication::arguments();
    for (int i = 1; i < args.length(); ++i){

        if (args[i] == "--hidetotray"){
            hide();
        }else if (args[i] == "--defaulteditor"){
            i++;
            if (i < args.length()){
                m_defaultEditor = args[i];
            }
            continue;
        }else if (args[i] == "--showtraymsg"){
            i++;
            if (i < args.length()){
                showTrayMessage("JSRoboKey Loaded", args[i]);
            }
            continue;
        }
        else if (QFile::exists(args[i])){
            //if the file exists, just load it
            bool scriptError = loadJSFile(args[i]);

            //TODO: if there was a script error, do we tell the user?

            //also do not load main script file
            loadMainScriptFile = false;

        }else{
            //TODO: not an existing file, is it something else like a command line option?
        }
    }//end looping through command line arguments

    if (loadMainScriptFile){
        loadMainScript();
    }

    //set the default editor for editing main file
#ifdef Q_OS_WIN32
    m_defaultEditor = "notepad";
#elif Q_OS_MAC
    m_defaultEditor = "TextEdit";
#else
    m_defaultEditor = "gedit";
#endif
    //TODO: the only REAL way to detect a valid editor is to spawn it and see if it returns false
}

const QStringList &DlgJsRoboKey::loadedModuleFileStack() const
{
    return m_loadedModuleFileStack;
}


bool DlgJsRoboKey::mainScriptLoaded() const
{
    return m_mainScriptLoaded;
}


void DlgJsRoboKey::loadMainScript(){
    QString mainScript = QDir::homePath() + "/jsrobokey_main.js";
    m_mainScriptLoaded = loadJSFile(mainScript);
    if (!m_mainScriptLoaded && !m_lastException.isEmpty()){
        showTrayMessage("Main Script had errors", m_lastException);
    }
    //remember, if the file doesn't exist we won't create it,
    //what if the user is running JsRoboKey portably?
}

/**
 * @brief DlgJsRoboKey::createAndEditMainScriptFile
 * Launch default text editor to edit the main script file
 */
void DlgJsRoboKey::createAndEditMainScriptFile()
{
    QString mainScript = QDir::homePath() + "/jsrobokey_main.js";
    QString command = m_defaultEditor;
    QStringList args;
    args << mainScript;
    QProcess p(this);
    if (!p.startDetached(command, args)){
        //TODO: iterate through a list of possible installed gui editors and start that instead
    }
    qDebug() << "spawned " << command << args;
}

void  DlgJsRoboKey::createActions()
{
    actEditMainScript = new QAction("Edit main script", this);
    connect(actEditMainScript, SIGNAL(triggered()), this, SLOT(createAndEditMainScriptFile()));

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
    trayIconMenu->addAction(actEditMainScript);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void DlgJsRoboKey::showTrayMessage(const QString& title, const QString& body, const QJSValue &callback,
                                   int iicon, int ms_duration,
                                   const QString& action, const QString& param1)
{

    //actions are to be defined in nature
    //TODO: notice, this will screw up the previous callback, does it matter?
    trayJsCallback = callback;
    trayAction = action;
    trayParam1 = param1;
    // NoIcon, Information, Warning, Critical
    QSystemTrayIcon::MessageIcon icon = (QSystemTrayIcon::MessageIcon)iicon;
    if (ms_duration == 0){ ms_duration = 3500; }
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
        showTrayMessage("JsRoboKey is still running.",
                        "To exit the program completely and all running scripts, Right click -> quit ");
        hide();
        event->ignore();
    }
}

void DlgJsRoboKey::on_trayMessageClicked()
{
    //TODO: FIX: this stupid trayJsCallback always ends up being undefined.
    //TODO: depending on the action we need to do something
    if (trayJsCallback.isCallable()){
        trayJsCallback.call();
        //it was already called, now make it undefined
    }
    trayJsCallback = QJSValue();
    //m_pjsrobokey->alert("tray clicked!!","");
}

void DlgJsRoboKey::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //restore the app
        showNormal();
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

void DlgJsRoboKey::showTrayMessage(const QString &title, const QString &body)
{
    QJSValue undefined;
    showTrayMessage(title, body, undefined);
}

DlgJsRoboKey::~DlgJsRoboKey()
{ 
    delete m_jsengine;
    delete m_pjsrobokey;
    delete ui;
    delete minimizeAction;
    delete actEditMainScript;
    delete restoreAction;
    delete maximizeAction;
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
    m_jsengine->globalObject().setProperty("__APPFILEPATH__", QCoreApplication::applicationFilePath());
    m_jsengine->globalObject().setProperty("__APPDIRPATH__", QCoreApplication::applicationDirPath());
    m_jsengine->globalObject().setProperty("__HOMEPATH__", QDir::homePath());


    //QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    //allow rk for short
    //make some functions global scope
    loadJS("jsrk = rk = JsRoboKey; "
           "alert = rk.alert; "
           "include = rk.include; "
           "require = rk.require", "JsRoboKey::initialize()");
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
    static int num_loadjs_calls = 0;
    num_loadjs_calls++;
    QString plural = (num_loadjs_calls > 1 ? "s" : "");
    trayIcon->setToolTip(tr("JsRoboKey (%1 loadJS call%2 made.)")
                         .arg(num_loadjs_calls).arg(plural));

    m_lastException = tr("");
    m_lastRunCode = code;
    m_lastRunFileOrModule = module_or_filename;
    m_loadedModuleFileStack.append(module_or_filename);
    //prior to evaluation we want to store the module_or_filename in __FILE__
    //that the script itself can use
    m_jsengine->globalObject().setProperty("__FILE__", QJSValue(module_or_filename));
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

void DlgJsRoboKey::on_btnEditMainScript_clicked()
{
    createAndEditMainScriptFile();
}

