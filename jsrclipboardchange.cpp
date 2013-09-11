#include "jsrclipboardchange.h"

#include <QApplication>
#include <QMimeData>


JSRClipboardChange::JSRClipboardChange(QJSEngine *pengine, QJSValue callback)
    : JSCallback(pengine, callback)
{
    //do nothing
}

bool JSRClipboardChange::exec()
{
    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(changed(QClipboard::Mode)),this,SLOT(changedSlot(QClipboard::Mode)));
    return true;
}




void JSRClipboardChange::changedSlot(QClipboard::Mode mode)
{
      //TODO: support images and other cool stuff like html, images, urls etc... @see QMimeData
      QJSValueList args;
      if(QApplication::clipboard()->mimeData()->hasText())
      {
          QString data = m_clipboard->text();
          args.push_back(QJSValue(data));
      }

      call(args);

      //emit a signal that our callback was completed, technically we should be able to
      //delete the object but i'm not sure if we have to wait a little bit
      emit callbackCompleted(this, m_callbackResult);
}
