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
    JsRoboKey* m_pjsrobokey;
};





#endif // DLGJSROBOKEY_H
