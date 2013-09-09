#include "jsrglobalhotkey.h"

#include "jscallback.h"

JSRGlobalHotkey::JSRGlobalHotkey(QJSEngine* pengine, QJSValue callback, const QString& shortcut)
    : JSCallback(pengine, callback), m_shortcut_string(shortcut), m_pshortcut(NULL)
{
    //do nothing
}

JSRGlobalHotkey::~JSRGlobalHotkey()
{
    delete m_pshortcut;
}

bool JSRGlobalHotkey::exec()
{
    m_pshortcut = new QxtGlobalShortcut();
    m_pshortcut->setShortcut(QKeySequence(m_shortcut_string));

    connect(m_pshortcut, SIGNAL(activated()), this, SLOT(shortcutExecuted()));
    m_pshortcut->setEnabled(true);
    return m_pshortcut->isEnabled();
}

QxtGlobalShortcut *JSRGlobalHotkey::shortcut() const
{
    return m_pshortcut;
}

QString JSRGlobalHotkey::shortcutString() const
{
    return m_shortcut_string;
}

void JSRGlobalHotkey::setShortcutString(const QString &shortcut_string)
{
    m_shortcut_string = shortcut_string;
}

void JSRGlobalHotkey::setShortcut(QxtGlobalShortcut *shortcut)
{
    m_pshortcut = shortcut;
}

void JSRGlobalHotkey::shortcutExecuted()
{
    QJSValueList args;
    //TODO: what data do we send back? the global shortcut was pressed

    call(args);

    //emit a signal that our callback was completed,
    //in this case, we should not allow our shortcut to be deleted because it is global shortcut that can be pressed multiple times
    emit callbackCompleted(this, m_callbackResult);
}

