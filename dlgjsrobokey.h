#ifndef DLGJSROBOKEY_H
#define DLGJSROBOKEY_H

#include <QDialog>
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

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
BOOL SendText( LPCTSTR lpctszText );
#endif

namespace Ui {
class DlgJsRoboKey;
}


typedef QPair<QxtGlobalShortcut*, QJSValue> QxtGlbShortcutJSVal;


//void setAttribute(QNetworkRequest::Attribute code, const QVariant &value);
class RKNetReply : public QNetworkReply {
     public:
       void setPointer(void* ptr){
           this->setAttribute(QNetworkRequest::User, QVariant((int)ptr));
       }

       void* getPointer(){
           return (void*)this->attribute(QNetworkRequest::User).toInt();
       }
};



class DlgJsRoboKey : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgJsRoboKey(QWidget *parent = 0);
    ~DlgJsRoboKey();
    

    void initialize();
    bool loadJSFile(const QString& file);
    bool loadJS(const QString& code, const QString &module_or_filename);




public slots:
    //------------------------------------------
    //-- Warning: Available to the js engine. --
    //------------------------------------------
    const QString clipboard();
    bool fileExists(const QString& file);
    bool requireOnce(const QString& file);
    bool include(const QString& file);
    bool addGlobalHotKey(const QString& hotkey, const QJSValue& callback);
    bool download(const QString& url, const QJSValue &callback);
    void helloWorld(){ qDebug() << "hello World"; alert("Hello world"); }
    void openUrl(const QString& url);
    bool run(const QString& file, const QString &a1="", const QString &a2="", const QString &a3="", const QString &a4="", const QString &a5="", const QString &a6="");
    QString runWait(const QString& file, const QString& a1="");
    QByteArray downloadedData() const;
    QString getIncludedFiles();

    void alert(const QString &text, const QString &title = "");

    bool sendKeys(const QString& keys);
    void sendVKey(WORD vk);

    int build();
    QString version();

    void sleep(int ms);
    bool exit();

    QString help();
    QString getMethods();
    //--------------------------------------------
    //yeah don't tell anyone you can call this
    void fileDownloaded(QNetworkReply *pReply);
private:
    QStringList m_included_files;
    QList<QxtGlbShortcutJSVal> m_globalHotkeys;
    QByteArray m_downloadedData;
    QMap<QString, QString> m_urlToCallback;
    QString m_version;
private slots:
    void on_btnInstaRun_clicked();

private:
    Ui::DlgJsRoboKey *ui;
    QJSEngine m_jsengine;
    QJSValue m_rk;
    QJSValue m_lastRunVal;
    QString m_lastException;
    QString m_lastRunCode;
    QString m_lastRunFileOrModule;

    QNetworkAccessManager m_webCtrl;
};



#endif // DLGJSROBOKEY_H
