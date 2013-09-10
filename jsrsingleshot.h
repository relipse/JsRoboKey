#ifndef JSRSINGLESHOT_H
#define JSRSINGLESHOT_H

#include "jscallback.h"

#include <QTimer>

class JSRSingleShot : public JSCallback
{
    Q_OBJECT
public:
    JSRSingleShot(QJSEngine* pengine, QJSValue callback, int ms);
    bool exec();

    int remainingTime();
    void cancel();
public slots:
    void timer();

private:
    int m_ms;
    QTimer m_timer;
};

#endif // JSRSINGLESHOT_H
