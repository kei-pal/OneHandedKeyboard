#include <windows.h>
#include <iostream>

HHOOK keyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        // Check for Key Down event
        if (wParam == WM_KEYDOWN) {
            // Get the virtual key code from the KBDLLHOOKSTRUCT
            DWORD vkCode = p->vkCode;

            // Optionally, convert the virtual key code to a character or a string
            // For demonstration, we'll just print the virtual key code
            std::cout << "Key Pressed: " << vkCode << std::endl;

            // If space is held down, you can implement your mirroring logic here
            // And optionally print out the mirrored key or action
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                std::cout << "Space is held down. Implement mirroring logic here." << std::endl;
            }

            
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main() {
    // Set the hook to monitor low-level keyboard input events
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    // Basic message loop to keep the application running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove the hook when the application exits
    UnhookWindowsHookEx(keyboardHook);

    return 0;
}
