#ifndef JSRGLOBALHOTKEY_H
#define JSRGLOBALHOTKEY_H

#include "jscallback.h"

#include <QxtWidgets/QxtGlobalShortcut>

class JSRGlobalHotkey : public JSCallback
{
    Q_OBJECT
private:
    QxtGlobalShortcut* m_pshortcut;
    QString m_shortcut_string;
public:
    JSRGlobalHotkey(QJSEngine* pengine, QJSValue callback, const QString& shortcut);
    ~JSRGlobalHotkey();
    bool exec();

    QxtGlobalShortcut *shortcut() const;
    void setShortcut(QxtGlobalShortcut* shortcut);

    QString shortcutString() const;
    void setShortcutString(const QString &shortcut_string);

public slots:
    void shortcutExecuted();
};

#endif // JSRGLOBALHOTKEY_H
