#include "systemkeyboardreadwrite.h"

SystemKeyboardReadWrite::SystemKeyboardReadWrite() :
    QObject()
{
    // Assign to null
    keyboardHook = NULL;
}

UINT SystemKeyboardReadWrite::ConvertKeyboardHookStruct(KBDLLHOOKSTRUCT *pKeyboard, QString& keyname)
{
    keyname = "";
    UINT virtualKey = pKeyboard->vkCode;
    UINT scanCode = pKeyboard->scanCode;
    DWORD flags = pKeyboard->flags;
    if (virtualKey == 255)
    {
      // discard "fake keys" which are part of an escaped sequence
      return 0;
    }
    else if (virtualKey == VK_SHIFT)
    {
      // correct left-hand / right-hand SHIFT
      virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
    }
    else if (virtualKey == VK_NUMLOCK)
    {
      // correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
      scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
    }

    // e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
    const bool isE0 = ((flags & RI_KEY_E0) != 0);
    const bool isE1 = ((flags & RI_KEY_E1) != 0);

    if (isE1)
    {
      // for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
      // however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
      if (virtualKey == VK_PAUSE)
        scanCode = 0x45;
      else
        scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
    }

    switch (virtualKey)
    {
      // right-hand CONTROL and ALT have their e0 bit set
      case VK_CONTROL:
        if (isE0)
          virtualKey = Keys::RIGHT_CONTROL;
        else
          virtualKey = Keys::LEFT_CONTROL;
        break;

      case VK_MENU:
        if (isE0)
          virtualKey = Keys::RIGHT_ALT;
        else
          virtualKey = Keys::LEFT_ALT;
        break;

      // NUMPAD ENTER has its e0 bit set
      case VK_RETURN:
        if (isE0)
          virtualKey = Keys::NUMPAD_ENTER;
        break;

      // the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys will always have their e0 bit set, but the
      // corresponding keys on the NUMPAD will not.
      case VK_INSERT:
        if (!isE0)
        virtualKey = Keys::NUMPAD_0;
        break;

      case VK_DELETE:
        if (!isE0)
          virtualKey = Keys::NUMPAD_DECIMAL;
        break;

      case VK_HOME:
        if (!isE0)
          virtualKey = Keys::NUMPAD_7;
        break;

      case VK_END:
        if (!isE0)
          virtualKey = Keys::NUMPAD_1;
        break;

      case VK_PRIOR:
        if (!isE0)
          virtualKey = Keys::NUMPAD_9;
        break;

      case VK_NEXT:
        if (!isE0)
          virtualKey = Keys::NUMPAD_3;
        break;

      // the standard arrow keys will always have their e0 bit set, but the
      // corresponding keys on the NUMPAD will not.
      case VK_LEFT:
        if (!isE0)
          virtualKey = Keys::NUMPAD_4;
        break;

      case VK_RIGHT:
        if (!isE0)
          virtualKey = Keys::NUMPAD_6;
        break;

      case VK_UP:
        if (!isE0)
          virtualKey = Keys::NUMPAD_8;
        break;

      case VK_DOWN:
        if (!isE0)
          virtualKey = Keys::NUMPAD_2;
        break;

      // NUMPAD 5 doesn't have its e0 bit set
      case VK_CLEAR:
        if (!isE0)
          virtualKey = Keys::NUMPAD_5;
        break;
    }
    // a key can either produce a "make" or "break" scancode. this is used to differentiate between down-presses and releases
    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
    const bool wasUp = ((flags & RI_KEY_BREAK) != 0);

    // getting a human-readable string
    UINT hkey = (scanCode << 16) | (isE0 << 24);
    wchar_t buffer[512] = {};
    GetKeyNameText((LONG)hkey, buffer, 512);
    keyname = QString::fromWCharArray(buffer);
    return scanCode;

}

LRESULT CALLBACK SystemKeyboardReadWrite::keyboardProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{

    // Check for a key down press
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_KEYDOWN)
        {
            KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
            QString keyname;
            UINT vk = ConvertKeyboardHookStruct(pKeyboard, keyname);

            emit SystemKeyboardReadWrite::instance()->keyPressed(keyname);
        }
        else if (wParam == WM_KEYUP)
        {
            KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
            QString keyname;
            UINT vk = ConvertKeyboardHookStruct(pKeyboard, keyname);
            emit SystemKeyboardReadWrite::instance()->keyReleased(keyname);
        }
        //reconnect
        instance()->setConnected( false );
        instance()->setConnected( true );
    }

    return false;
}

bool SystemKeyboardReadWrite::connected()
{
    return keyboardHook;
}

bool SystemKeyboardReadWrite::setConnected(bool state)
{
    if(state && keyboardHook == NULL)
    {
        keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProcedure, GetModuleHandle(NULL), 0);

        return keyboardHook;
    }
    else
    {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = NULL;

        return keyboardHook;
    }
}

SystemKeyboardReadWrite* SystemKeyboardReadWrite::instance()
{
    static SystemKeyboardReadWrite* pKeyboardReadWriteInstance = new SystemKeyboardReadWrite();
    return pKeyboardReadWriteInstance;
}
