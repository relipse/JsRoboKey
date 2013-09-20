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
#include <QSystemTrayIcon>
#include <QMenu>

#include "jsrobokey.h"
class JsRoboKey;

namespace Ui {
class DlgJsRoboKey;
}

#include "jsrobokey.h"
class JsRoboKey;

class DlgJsRoboKey : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgJsRoboKey(QWidget *parent = 0);
    ~DlgJsRoboKey();

    void initialize();
    bool loadJSFile(const QString& file);
    bool loadJS(const QString& code, const QString &module_or_filename);
    QJSEngine* jsengine();

    void setIcon(int index = 1);

    void showTrayMessage(const QString &title, const QString &body);
    void showTrayMessage(const QString &title, const QString &body, const QJSValue &callback, int iicon = 0,
                         int ms_duration = 3500, const QString &action="", const QString &param1="");

    void setVisible(bool visible);
    void createTrayIcon();
    void createActions();
    void loadMainScript();
    bool mainScriptLoaded() const;

    const QStringList &loadedModuleFileStack() const;
public slots:
    void createAndEditMainScriptFile();
private slots:
    void on_btnInstaRun_clicked();
    void on_btnUnloadAll_clicked();
    void on_trayMessageClicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_btnEditMainScript_clicked();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:

    Ui::DlgJsRoboKey *ui;
    QJSEngine* m_jsengine;
    QJSValue m_rk;
    QJSValue m_lastRunVal;
    QString m_lastException;
    QString m_lastRunCode;
    QString m_lastRunFileOrModule;
    JsRoboKey* m_pjsrobokey;

    QAction* actEditMainScript;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;


    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QString trayAction;
    QString trayParam1;
    QJSValue trayJsCallback;

    QString m_defaultEditor;
    bool m_mainScriptLoaded;

    QStringList m_loadedModuleFileStack;
};
#endif // DLGJSROBOKEY_H
