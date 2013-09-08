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

DlgJsRoboKey::DlgJsRoboKey(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgJsRoboKey)
{
    ui->setupUi(this);

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
    m_version = QDate::fromString(__DATE__, "MMM dd yyyy").toString("yyyy-MM-dd");
}

DlgJsRoboKey::~DlgJsRoboKey()
{ 
    /*
    //remove all hotkeys
    for (int i = 0; i < m_globalHotkeys.size(); ++i){
        m_globalHotkeys[i].first->setEnabled(false);
        delete m_globalHotkeys[i].first;
        m_globalHotkeys[i].first = NULL;
    }
    delete ui;*/
}

/**
 * @brief DlgJsRoboKey::initialize
 *  Initialize all global objects and variables
 */
void DlgJsRoboKey::initialize()
{
    m_rk = m_jsengine.newQObject(this);
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

const QString DlgJsRoboKey::clipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();
    return originalText;
}

bool DlgJsRoboKey::fileExists(const QString &file)
{
    QFile f(file);
    return f.exists();
}

bool DlgJsRoboKey::requireOnce(const QString &file)
{
    if (m_included_files.contains(file)){
        return false;
    }
    m_included_files.push_back(file);
    return loadJSFile(file);
}

//include a file with no regard if it has already
//been included
bool DlgJsRoboKey::include(const QString &file)
{
    if (!m_included_files.contains(file)){
        m_included_files.push_back(file);
    }
    return loadJSFile(file);
}

bool DlgJsRoboKey::addGlobalHotKey(const QString &hotkey, const QJSValue &callback)
{
    //TODO: make callback work
    //TODO: check if we have an existing hotkey
    if (!callback.isCallable()){
        return false;
    }
    QxtGlobalShortcut* shortcut = new QxtGlobalShortcut();
    m_globalHotkeys.push_back(qMakePair(shortcut, callback));
    shortcut->setShortcut(QKeySequence(hotkey));
    //TODO: actually call javascript engine callback instead of helloWorld
    connect(shortcut, SIGNAL(activated()), this, SLOT(helloWorld()));
    shortcut->setEnabled(true);
}


bool DlgJsRoboKey::download(const QString &url, const QJSValue &callback)
{
    if (!callback.isCallable()){
         return false;
    }
    qDebug() << "downloading " << url << " callback: " << callback.toString();
    connect(&m_webCtrl, SIGNAL(finished(QNetworkReply*)),
            SLOT(fileDownloaded(QNetworkReply*)));
    QUrl u(url);
    QNetworkRequest request(u);
    RKNetReply* reply = (RKNetReply*)m_webCtrl.get(request);
    QJSValue* nwCallback = new QJSValue(callback);
    reply->setPointer((void*)nwCallback);
    return true;
}

void DlgJsRoboKey::openUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

bool DlgJsRoboKey::run(const QString &file, const QString& a1, const QString& a2, const QString& a3, const QString& a4, const QString& a5, const QString& a6)
{
    QProcess *process = new QProcess(this);
    QStringList args;
    if (!a1.isEmpty()){
        args << a1;
    }
    if (!a2.isEmpty()){
        args << a2;
    }
    if (!a3.isEmpty()){
        args << a3;
    }
    if (!a4.isEmpty()){
        args << a4;
    }
    if (!a5.isEmpty()){
        args << a5;
    }
    if (!a6.isEmpty()){
        args << a6;
    }
    process->start(file, args);
}

QString DlgJsRoboKey::runWait(const QString &file, const QString &a1)
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(file, QIODevice::ReadWrite);

    // Wait for it to start
    if(!process.waitForStarted())
        return 0;

    // Continue reading the data until EOF reached
    QByteArray data;

    while(process.waitForReadyRead())
        data.append(process.readAll());

    QString s(data);
    return s;
}


void DlgJsRoboKey::fileDownloaded(QNetworkReply* pReply)
{
    m_downloadedData = pReply->readAll();
    QString data(m_downloadedData);

    //QVariant vcallback = pReply->attribute(QNetworkRequest::User);

    QJSValue* cb = (QJSValue*)(void*)pReply->attribute(QNetworkRequest::User).toInt();

    qDebug() << cb->toString(); //<< "Downloaded file data: " << data;

    if (cb->isCallable()){
        QJSValueList args;
        args.push_back(QJSValue(QString(m_downloadedData)));
       cb->call(args);
    }


    //emit a signal
    pReply->deleteLater();

    delete pReply;
    //emit downloaded();

    //finally we delete it after the download came through
    //TODO: if the download never comes through we are screwed!
    delete cb;
}

void DlgJsRoboKey::on_btnInstaRun_clicked()
{
    loadJS(ui->memoInstaScript->toPlainText(), "instarun");
    ui->lblStatus->setText(m_lastRunVal.toString());
}


QByteArray DlgJsRoboKey::downloadedData() const
{
    return m_downloadedData;
}

QString DlgJsRoboKey::getIncludedFiles()
{
    //they are displayed in the order they were included
    //with the exception if it was included twice the 2nd one does not appear
    QString s = "";
    for (int i = 0; i < m_included_files.length();++i){
        if (s != ""){ s += ", "; }
        s += m_included_files[i];
    }
    return s;
}

void DlgJsRoboKey::alert(const QString &text, const QString& title)
{
    QString t = title;
    if (t.isEmpty()){
        t = "JsRoboKey Alert";
    }
    QMessageBox::information(this, t, text);
}


void DlgJsRoboKey::sendVKey(WORD vk){
    INPUT ip;

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the key down
    ip.ki.wVk = vk;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

bool DlgJsRoboKey::sendKeys(const QString &keys)
{
    /*
    for (int i = 0; i < keys.length();++i){
        char c = keys[i].toLatin1();
        //TODO: map key to actual virtual key code
        sendVKey((WORD)c);
    } */
    #ifdef WIN32
    wchar_t myArray[keys.size()+1];
    int x;
    for (x=0; x < keys.size(); x++)
    {
        myArray[x] = (wchar_t)keys.at(x).toLatin1();
    }
    myArray[x] = '\0';

    LPCTSTR str = myArray;
    return SendText(str);
    #endif
    //TODO: make this cross platform
        return false;
}

#ifdef WIN32
#include <string.h>
#include <tchar.h>
BOOL SendText( LPCTSTR lpctszText )
    {
    std::vector<INPUT> EventQueue;

    TCHAR Buff[120 * sizeof(TCHAR)] = {0};
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILANGUAGE, Buff, sizeof(Buff));
    HKL hKeyboardLayout = ::LoadKeyboardLayout( Buff, KLF_ACTIVATE );

    const size_t Len = wcslen( lpctszText );
    for( size_t Index = 0; Index < Len; ++Index )
        {
        INPUT Event = { 0 };

        const SHORT Vk = VkKeyScanEx(lpctszText[Index], hKeyboardLayout);
        const UINT VKey = ::MapVirtualKey( LOBYTE( Vk ), 0 );

        if( HIBYTE( Vk ) == 1 ) // Check if shift key needs to be pressed for this key
            {
            // Press shift key
            ::ZeroMemory( &Event, sizeof( Event ));
            Event.type = INPUT_KEYBOARD;
            Event.ki.dwFlags = KEYEVENTF_SCANCODE;
            Event.ki.wScan = ::MapVirtualKey( VK_LSHIFT, 0 );
            EventQueue.push_back( Event );
            }

        // Keydown
        ::ZeroMemory( &Event, sizeof( Event ));
        Event.type = INPUT_KEYBOARD;
        Event.ki.dwFlags = KEYEVENTF_SCANCODE;
        Event.ki.wScan = VKey;
        EventQueue.push_back( Event );

        // Keyup
        ::ZeroMemory( &Event, sizeof( Event ));
        Event.type = INPUT_KEYBOARD;
        Event.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        Event.ki.wScan = VKey;
        EventQueue.push_back( Event );

        if( HIBYTE( Vk ) == 1 )// Release if previously pressed
            {
            // Release shift key
            ::ZeroMemory( &Event, sizeof( Event ));
            Event.type = INPUT_KEYBOARD;
            Event.ki.dwFlags = KEYEVENTF_SCANCODE| KEYEVENTF_KEYUP;
            Event.ki.wScan = ::MapVirtualKey( VK_LSHIFT, 0 );
            EventQueue.push_back( Event );
            }
        }// End for

    if( hKeyboardLayout )
        {
        UnloadKeyboardLayout( hKeyboardLayout );
        }

    return static_cast<BOOL>(::SendInput( static_cast<UINT>(EventQueue.size()), &EventQueue[0], sizeof( INPUT )));
}
#endif

void DlgJsRoboKey::sleep(int ms)
{
    if (ms < 0){ return; }
    #ifdef Q_OS_WIN
        Sleep(uint(ms));
    #else
        struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
    #endif
}

int DlgJsRoboKey::build()
{
    return 15;
}


QString DlgJsRoboKey::version()
{
    return m_version;
}

bool DlgJsRoboKey::exit()
{
    this->close();
    return true;
}

QString DlgJsRoboKey::help()
{
    return "Available Methods:\n"
            + getMethods();
}

/**
 * @brief DlgJsRoboKey::getMethods
 * Get a string showing all of the methods available to connect
 * @return
 */
QString DlgJsRoboKey::getMethods()
{
    QString s = "";
    //DlgJsRoboKey *obj = static_cast<DlgJsRoboKey*>(DlgJsRoboKey::staticMetaObject.newInstance());
    for (int i = 0; i < this->metaObject()->methodCount(); i++)
    {
        QMetaMethod method = this->metaObject()->method(i);
        if (method.methodType() == QMetaMethod::Slot && method.access() == QMetaMethod::Public){
            if (s != ""){ s += "\n"; }
            s += method.methodSignature();
        }
    }
    return s;
}



