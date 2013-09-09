#ifndef JSRDOWNLOAD_H
#define JSRDOWNLOAD_H

#include "jscallback.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class JSRDownload : public JSCallback {
    Q_OBJECT
private:
    QString m_url;
    QNetworkAccessManager m_webCtrl;
    QNetworkRequest m_request;
    QNetworkReply* m_reply;
public:
    JSRDownload(QJSEngine* pengine, QJSValue callback, const QString& url)
        : JSCallback(pengine, callback), m_url(url)
    { }

    bool exec();
public slots:
    void fileDownloaded(QNetworkReply *pReply);
};

#endif // JSRDOWNLOAD_H
