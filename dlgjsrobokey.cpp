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
    m_pjsrobokey(NULL)
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

    initialize();
}

DlgJsRoboKey::~DlgJsRoboKey()
{ 
    delete m_pjsrobokey;
    delete ui;
}

/**
 * @brief DlgJsRoboKey::initialize
 *  Initialize all global objects and variables
 */
void DlgJsRoboKey::initialize()
{
    m_rk = m_jsengine.newQObject(m_pjsrobokey);
    m_jsengine.globalObject().setProperty("JsRoboKey", m_rk);

    //allow rk for short
    loadJS("jsrk = rk = JsRoboKey;", "JsRoboKey::initialize()");
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
    m_lastRunVal = m_jsengine.evaluate(code, module_or_filename);
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
   return &m_jsengine;
}






