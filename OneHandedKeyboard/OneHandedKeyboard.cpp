#include <windows.h>
#include <iostream>
#include <chrono>
#include <map>

using Clock = std::chrono::high_resolution_clock;
std::chrono::time_point<Clock> spacePressedTime;
bool spaceHeld = false;
std::map<DWORD, DWORD> keyMappings;

bool SendMirroredKeyPress(UINT vkCode);
void InitializeKeyMappings();

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
        if (kbdStruct.vkCode == VK_SPACE) {
            switch (wParam) {
            case WM_KEYDOWN:
                if (!spaceHeld) { // Check if this is the first event of space bar press
                    spacePressedTime = Clock::now();
                    spaceHeld = true;
                }
                return 1; // Block this event to prevent default space input
            case WM_KEYUP:
                if (spaceHeld) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - spacePressedTime).count();
                    if (elapsed < 200) {
                        // It was a tap, simulate a space key press
                        INPUT input[2] = {};
                        input[0].type = INPUT_KEYBOARD;
                        input[0].ki.wVk = VK_SPACE;
                        input[1].type = INPUT_KEYBOARD;
                        input[1].ki.wVk = VK_SPACE;
                        input[1].ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(2, input, sizeof(INPUT));
                    }
                    spaceHeld = false;
                    return 1;
                }
                break;
            }
        }
        else {
            // For other keys, check if space is held (mirror mode active) and handle accordingly
            if (spaceHeld && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
                if (SendMirroredKeyPress(kbdStruct.vkCode)) {
                    return 1; // Block the original key press
                }
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool SendMirroredKeyPress(UINT vkCode) {
    auto it = keyMappings.find(vkCode);
    if (it != keyMappings.end()) {
        INPUT input[1] = {};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = it->second;

        SendInput(1, input, sizeof(INPUT));

        return true;
    }
    return false;
}

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

int main() {
    InitializeKeyMappings();

    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    std::cout << "Hook set. Press and hold space to enter mirror mode. Tap space for normal input." << std::endl;

    MSG msg;
    while (!GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);
    return 0;
}
