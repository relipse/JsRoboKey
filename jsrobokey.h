#ifndef JSROBOKEY_H
#define JSROBOKEY_H

#include <QObject>

#include <QJSEngine>
#include <QxtWidgets/QxtGlobalShortcut>
#include <QtQml/QJsEngine>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QMessageBox>
#include <QSignalMapper>
#include <QPair>
#include <QNetworkReply>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include "dlgjsrobokey.h"

#include "jscallback.h"
#include "jsrglobalhotkey.h"
#include "jsrsingleshot.h"

class DlgJsRoboKey;

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
BOOL SendText( LPCTSTR lpctszText );
#endif


//---------------------------------------------------------------------
//-- Warning: All public slot methods are available to the js engine. --
//-----------------------------------------------------------------------
class JsRoboKey : public QObject
{
    Q_OBJECT
public:
    explicit JsRoboKey(QObject *parent = 0);
    ~JsRoboKey();
    DlgJsRoboKey* app(){ return (DlgJsRoboKey*)parent(); }



signals:
    
public slots:
        const QString clipboard();
        bool fileExists(const QString& file);
        bool require(const QString& file);
        bool include(const QString& file);
        bool addGlobalHotkey(const QString& hotkey, const QJSValue& callback);
        bool download(const QString& url, const QJSValue &callback_complete);
        void openUrl(const QString& url);
        void openFile(const QString& file);
        void open(const QString& url);

        bool run(const QString& file, const QString &a1="", const QString &a2="", const QString &a3="", const QString &a4="", const QString &a5="", const QString &a6="");
        QString runWait(const QString& file, const QString& a1="");
        QString getIncludedFiles();

        void alert(const QString &text, const QString &title = "");
        int setTimeout(const QJSValue& callback, int ms);
        int timeoutRemainingTime(int timeoutId);
        void clearTimeout(int timeoutId);

        bool sendKeys(const QString& keys);
        void sendVKey(WORD vk);

        int build();
        QString version(){ return "7777777777"; }

        void sleep(int ms);
        bool exit();

        QString compilationDate();
        QString help();
        QString getMethods();

        //more windows api stuff
        int getForegroundWindow();
        QString getWindowText(int hwnd);

        //---what-the-hey-we-gotta-have-one-of-these
        void helloWorld(){ qDebug() << "hello World"; alert("Hello world"); }
        //--------------------------------------------
private:

        QStringList m_included_files;
        /**
         * @brief a list of callbacks that need to be deleted sometime, either after they get
         *  called if they are temporary or after JsRoboKey exits
         */
        QList<JSCallback*> m_callbacks;


        QMap<int, JSRSingleShot*> m_timers;
};

#endif // JSROBOKEY_H
