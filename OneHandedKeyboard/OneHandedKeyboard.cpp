#include <windows.h>
#include <iostream>
#include <map>

HHOOK keyboardHook;
std::map<DWORD, DWORD> keyMappings;

void InitializeKeyMappings() {
    // top row
    keyMappings[0xBD] = 0xC0; // - to `
    keyMappings[0x30] = 0x31; // 0 to 1
    keyMappings[0x39] = 0x32; // 9 to 2
    keyMappings[0x38] = 0x33; // 8 to 3
    keyMappings[0x37] = 0x34; // 7 to 4
    keyMappings[0x36] = 0x35; // 6 to 5

    // second row
    keyMappings[0xDC] = 0x09; // \ to tab <- not sure
    keyMappings[0x50] = 0x51; // p to q
    keyMappings[0x4F] = 0x57; // o to w
    keyMappings[0x49] = 0x45; // i to e
    keyMappings[0x55] = 0x52; // u to 
    keyMappings[0x59] = 0x54; // y to t

    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
    keyMappings[0x] = 0x; //
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        // Check if space is held down
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            auto it = keyMappings.find(p->vkCode);
            if (it != keyMappings.end()) {
                // Prepare a KEYBDINPUT structure for the mirrored key
                INPUT input[1] = {};
                input[0].type = INPUT_KEYBOARD;
                input[0].ki.wVk = it->second; // Mirrored virtual key code

                // Send the mirrored key press
                SendInput(1, input, sizeof(INPUT));

                // Optionally, block the original key press by returning a non-zero value
                return 1;
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main() {
    InitializeKeyMappings();

    // Set the hook
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
