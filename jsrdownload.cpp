
#include "jscallback.h"
#include "jsrdownload.h"


bool JSRDownload::exec()
{
    connect(&m_webCtrl, SIGNAL(finished(QNetworkReply*)),
            SLOT(fileDownloaded(QNetworkReply*)));
    QUrl u(m_url);
    QNetworkRequest m_request(u);
    m_reply = m_webCtrl.get(m_request);
    return true;
}

void JSRDownload::fileDownloaded(QNetworkReply *pReply)
{
    QByteArray downloadedData = pReply->readAll();
    QString data(downloadedData);
    QJSValueList args;
    args.push_back(QJSValue(data));

    call(args);

    pReply->deleteLater();

    //emit a signal that our callback was completed, technically we should be able to
    //delete the object but i'm not sure if we have to wait a little bit or a segfault might occur
    emit callbackCompleted(this, m_callbackResult);
}


