#include <windows.h>
#include <iostream>
#include <map>

HHOOK keyboardHook;
std::map<DWORD, DWORD> keyMappings;
bool isSpacePressed = false;
bool isOtherKeyPressed = false;

static void InitializeKeyMappings() {
    // 6 row
    keyMappings[0xBD] = 0xC0; // - to `
    keyMappings[0x30] = 0x31; // 0 to 1
    keyMappings[0x39] = 0x32; // 9 to 2
    keyMappings[0x38] = 0x33; // 8 to 3
    keyMappings[0x37] = 0x34; // 7 to 4
    keyMappings[0x36] = 0x35; // 6 to 5

    // Y row
    keyMappings[0xDC] = 0x09; // \ to tab
    keyMappings[0x50] = 0x51; // P to Q
    keyMappings[0x4F] = 0x57; // O to W
    keyMappings[0x49] = 0x45; // I to E
    keyMappings[0x55] = 0x52; // U to R
    keyMappings[0x59] = 0x54; // Y to T

    // H row
    keyMappings[0xBA] = 0x41; // ; to A
    keyMappings[0x4C] = 0x53; // L to S
    keyMappings[0x4B] = 0x44; // K to D
    keyMappings[0x4A] = 0x46; // J to F
    keyMappings[0x48] = 0x47; // H to G

    // B row
    keyMappings[0xBF] = 0x5A; // / to Z
    keyMappings[0xBE] = 0x58; // . to X
    keyMappings[0xBC] = 0x43; // , to C
    keyMappings[0x4D] = 0x56; // M to V
    keyMappings[0x4E] = 0x42; // N to B
}

LRESULT CALLBACK KeyboardProc(
    int nCode, 
    WPARAM wParam, 
    LPARAM lParam
) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        if (wParam == WM_KEYDOWN) {
            if (!isSpacePressed) {
                if (p->vkCode == VK_SPACE) {
                    isSpacePressed = true;
                    return 1;
                }
            }
            else {
                isOtherKeyPressed = true;

                // cancel repeat spaces
                if (p->vkCode == VK_SPACE) {
                    isSpacePressed = true;
                    return 1;
                }

                // mirror input
                auto it = keyMappings.find(p->vkCode);
                if (it != keyMappings.end()) {
                    // Prepare a KEYBDINPUT structure for the mirrored key
                    INPUT input[1] = {};
                    input[0].type = INPUT_KEYBOARD;
                    input[0].ki.wVk = it->second; // Mirrored virtual key code

                    // Send the mirrored key press
                    SendInput(1, input, sizeof(INPUT));

                    // Block the original key press by returning a non-zero value
                    return 1;
                }
            }
        }
        else if (wParam == WM_KEYUP) {
            if (p->vkCode == VK_SPACE) {
                if (isOtherKeyPressed) {
                    isSpacePressed = false;
                    isOtherKeyPressed = false;
                    return 1;
                }
                else {
                    // allow space to only happen here
                }
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

//if (nCode >= 0 && wParam == WM_KEYDOWN) {
//    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

//    // Check if space is held down
//    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
//        auto it = keyMappings.find(p->vkCode);
//        if (it != keyMappings.end()) {
//            // Prepare a KEYBDINPUT structure for the mirrored key
//            INPUT input[1] = {};
//            input[0].type = INPUT_KEYBOARD;
//            input[0].ki.wVk = it->second; // Mirrored virtual key code

//            // Send the mirrored key press
//            SendInput(1, input, sizeof(INPUT));

//            // Optionally, block the original key press by returning a non-zero value
//            return 1;
//        }
//    }
//}