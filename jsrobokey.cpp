#include "jsrobokey.h"

#include "jscallback.h"
#include "jsrdownload.h"

#include <QApplication>
#include <QTime>
#include <QClipboard>
#include <QMetaMethod>
#include <QFile>
#include <QProcess>
#include <QMessageBox>


JsRoboKey::JsRoboKey(QObject *parent) :
    QObject(parent)
{
}

JsRoboKey::~JsRoboKey()
{
    for (int i = 0; i < m_callbacks.size(); ++i){
        delete m_callbacks[i];
    }
}

const QString JsRoboKey::clipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();
    return originalText;
}


void JsRoboKey::sleep(int ms)
{
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while( QTime::currentTime() < dieTime ){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

int JsRoboKey::build()
{
    return 0;
}


bool JsRoboKey::exit()
{
    return true;
}

QString JsRoboKey::compilationDate()
{
    return QDate::fromString(__DATE__, "MMM dd yyyy").toString("yyyy-MM-dd");
}

QString JsRoboKey::help()
{
    return "Available Methods:\n"
            + getMethods();
}

/**
 * @brief JsRoboKey::getMethods
 * Get a string showing all of the methods available to connect
 * @return
 */
QString JsRoboKey::getMethods()
{
    QString s = "";
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

int JsRoboKey::getForegroundWindow()
{
    //TODO: make this cross platform
    #ifdef WIN32
    return (int)GetForegroundWindow();
    #endif
}

QString JsRoboKey::getWindowText(int hwnd)
{
    //TODO: make this cross platform
    QString s = "";
    #ifdef WIN32
    wchar_t buffer[256];
    int len = GetWindowText((HWND)hwnd, buffer, 255);
    s = QString::fromWCharArray(buffer);
    #endif
    return s;
}

bool JsRoboKey::fileExists(const QString &file)
{
    QFile f(file);
    return f.exists();
}

bool JsRoboKey::require(const QString &file)
{
    if (m_included_files.contains(file)){
        return false;
    }
    m_included_files.push_back(file);
    return app()->loadJSFile(file);
}

//include a file with no regard if it has already
//been included
bool JsRoboKey::include(const QString &file)
{
    if (!m_included_files.contains(file)){
        m_included_files.push_back(file);
    }
    return app()->loadJSFile(file);
}

bool JsRoboKey::addGlobalHotkey(const QString &hotkey, const QJSValue &callback)
{
    JSCallback* pcallback = new JSRGlobalHotkey(app()->jsengine(), callback, hotkey);
    m_callbacks.push_back(pcallback);
    return pcallback->exec();
}



bool JsRoboKey::download(const QString &url, const QJSValue &callback_complete)
{
    JSCallback* jscb = new JSRDownload(app()->jsengine(), callback_complete, url);
    m_callbacks.push_back(jscb);
    //start the download, the callback will happen
    return jscb->exec();
}

void JsRoboKey::openUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

bool JsRoboKey::run(const QString &file, const QString& a1, const QString& a2, const QString& a3, const QString& a4, const QString& a5, const QString& a6)
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

QString JsRoboKey::runWait(const QString &file, const QString &a1)
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




QString JsRoboKey::getIncludedFiles()
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

void JsRoboKey::alert(const QString &text, const QString& title)
{
    QString t = title;
    if (t.isEmpty()){
        t = "JsRoboKey Alert";
    }
    QMessageBox::information(app(), t, text);
}

#include "jsrsingleshot.h"

int JsRoboKey::setTimeout(const QJSValue &callback, int ms)
{
    static int timerId = 1;
    JSRSingleShot* jscb = new JSRSingleShot(app()->jsengine(), callback, ms);
    m_timers[timerId++] = jscb;
    m_callbacks.push_back((JSCallback*)jscb);
    //start the timeout
    jscb->exec();
    return timerId-1;
}

int JsRoboKey::timeoutRemainingTime(int timeoutId){

    if (m_timers.contains(timeoutId)){
        return m_timers[timeoutId]->remainingTime();
    }
    return -3;
}

void JsRoboKey::clearTimeout(int timeoutId){
    JSCallback* pcb = NULL;

    if (m_timers.contains(timeoutId)){
        pcb = m_timers[timeoutId];
        m_timers[timeoutId]->cancel();
        m_timers[timeoutId]->deleteLater();
    }

    if (pcb){
        for (int i = 0; i < m_callbacks.size(); ++i){
            if (m_callbacks[i] == pcb){
                m_callbacks[i] = NULL;
            }
        }
    }
}


void JsRoboKey::sendVKey(WORD vk){
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

bool JsRoboKey::sendKeys(const QString &keys)
{
    //TODO: make this cross platform
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


