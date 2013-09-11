#ifndef JSRCLIPBOARDCHANGE_H
#define JSRCLIPBOARDCHANGE_H

#include "jscallback.h"

#include <QClipboard>

class JSRClipboardChange : public JSCallback
{
    Q_OBJECT
public:
    JSRClipboardChange(QJSEngine* pengine, QJSValue callback);
    bool exec();
public slots:
    void changedSlot(QClipboard::Mode mode);

protected:
    QClipboard* m_clipboard;
};

#endif // JSRCLIPBOARDCHANGE_H
