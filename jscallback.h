#ifndef JSCALLBACK_H
#define JSCALLBACK_H

#include <QObject>
#include <QJSEngine>
#include <QJSValue>

/**
 * @brief The JSCallback clas which
 *  allows execution, after execution it will
 *  call the callback function. You must extend this interface
 *  to make good use of it.
 */
class JSCallback : public QObject {
public:
    JSCallback() : m_pengine(NULL), m_callback(0), m_callbackResult(0) {}
    //override this function to initiate some events that will later call the callback
    virtual bool exec() = 0;
    QJSEngine *pengine() const;
    void setEngine(QJSEngine *pengine);

    QJSValue callback() const;
    void setCallback(const QJSValue &callback);

    //call the callback, unless it is a string then evaluate it and store the result
    //in m_callbackResult
    void call(const QJSValueList &args);

signals:
    void callbackCompleted(JSCallback* caller, const QJSValue& val);

protected:
    QJSEngine* m_pengine;
    QJSValue m_callback;
    QJSValue m_callbackResult;
};

#endif // JSCALLBACK_H
