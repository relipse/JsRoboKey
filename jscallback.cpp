#include "jscallback.h"

QJSEngine *JSCallback::pengine() const
{
  return m_pengine;
}

void JSCallback::setEngine(QJSEngine *pengine)
{
    m_pengine = pengine;
}

QJSValue JSCallback::callback() const
{
    return m_callback;
}

void JSCallback::setCallback(const QJSValue &callback)
{
    m_callback = callback;
}

void JSCallback::call(const QJSValueList& args)
{
    if (m_callback.isCallable()){
        m_callbackResult = m_callback.call(args);
    }else{ //not callable, just try to execute it as a string
        m_callbackResult = m_pengine->evaluate(m_callback.toString());
    }
}
