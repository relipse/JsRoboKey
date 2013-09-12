#pragma once
#include <QObject>
#include <windows.h>

namespace Keys {
    const int RIGHT_CONTROL = VK_RCONTROL;
    const int LEFT_CONTROL = VK_LCONTROL;
    const int RIGHT_ALT = VK_RMENU;
    const int LEFT_ALT = VK_LMENU;
    const int NUMPAD_ENTER = VK_RETURN;
    const int NUMPAD_0 = VK_NUMPAD0;
    const int NUMPAD_DECIMAL = VK_DECIMAL;
    const int NUMPAD_7 = VK_NUMPAD7;
    const int NUMPAD_1 = VK_NUMPAD1;
    const int NUMPAD_9 = VK_NUMPAD9;
    const int NUMPAD_3 = VK_NUMPAD3;
    const int NUMPAD_4 = VK_NUMPAD4;
    const int NUMPAD_6 = VK_NUMPAD6;
    const int NUMPAD_8 = VK_NUMPAD8;
    const int NUMPAD_2 = VK_NUMPAD2;
    const int NUMPAD_5 = VK_NUMPAD5;
}

class SystemKeyboardReadWrite : public QObject
{

Q_OBJECT
public:
    // Returns singleton instance
    static SystemKeyboardReadWrite * instance();

    // Returns whether the keyboard hook is connected
    bool connected();
    // Connects / Disconnects the keyboard hook
    bool setConnected(bool state);
signals:
    // Broadcasts a key has been pressed
    void keyPressed(const QString& keyname);
    // Broadcasts a key has been released
    void keyReleased(const QString& keyname);

private:
    static UINT ConvertKeyboardHookStruct(KBDLLHOOKSTRUCT *pKeyboard, QString& keyname);

    // Keyboard hook
    HHOOK keyboardHook;

    // Class constructor
    SystemKeyboardReadWrite();

    // Identifies hook activityaaaa
    static LRESULT CALLBACK keyboardProcedure(int nCode, WPARAM wParam, LPARAM lParam);

};
