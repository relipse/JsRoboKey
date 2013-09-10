#include "jsrsingleshot.h"

#include "jscallback.h"

#include <QTimer>

JSRSingleShot::JSRSingleShot(QJSEngine *pengine, QJSValue callback, int ms)
    : JSCallback(pengine, callback), m_ms(ms), m_timer(this)
{
}


bool JSRSingleShot::exec()
{
    //QTimer::singleShot(m_ms, this, SLOT(timer()));

    m_timer.setInterval(m_ms);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), SLOT(timer()));
    m_timer.start();
}

int JSRSingleShot::remainingTime()
{
   return m_timer.remainingTime();
}

void JSRSingleShot::cancel()
{
    m_timer.stop();
}

void JSRSingleShot::timer()
{
    QJSValueList args;
    //TODO: what data do we send back? the shot was fired

    call(args);

    //emit a signal that our callback was completed,
    //in this case, we should not allow our shortcut to be deleted because it is global shortcut that can be pressed multiple times
    emit callbackCompleted(this, m_callbackResult);
}
