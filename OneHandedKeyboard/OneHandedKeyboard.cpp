#include <windows.h>
#include <iostream>
#include <map>

HHOOK keyboardHook;
std::map<DWORD, DWORD> keyMappings;

// Initialize key mappings for mirroring
void InitializeKeyMappings() {
    // Example mappings: A -> L, S -> K, D -> J, etc.
    keyMappings[0x41] = 0x4C; // 'A' to 'L'
    keyMappings[0x53] = 0x4B; // 'S' to 'K'
    // Add more mappings as needed
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
