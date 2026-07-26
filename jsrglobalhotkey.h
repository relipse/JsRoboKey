#ifndef JSRGLOBALHOTKEY_H
#define JSRGLOBALHOTKEY_H

#include "jscallback.h"

#ifdef Q_OS_WIN
#include <QxtWidgets/QxtGlobalShortcut>
#endif

class JSRGlobalHotkey : public JSCallback
{
    Q_OBJECT
private:
#ifdef Q_OS_WIN
    QxtGlobalShortcut* m_pshortcut;
#endif
    QString m_shortcut_string;
public:
    JSRGlobalHotkey(QJSEngine* pengine, QJSValue callback, const QString& shortcut);
    ~JSRGlobalHotkey();
    bool exec();

#ifdef Q_OS_WIN
    QxtGlobalShortcut *shortcut() const;
    void setShortcut(QxtGlobalShortcut* shortcut);
#endif

    QString shortcutString() const;
    void setShortcutString(const QString &shortcut_string);

public slots:
    void shortcutExecuted();
};

#endif // JSRGLOBALHOTKEY_H
