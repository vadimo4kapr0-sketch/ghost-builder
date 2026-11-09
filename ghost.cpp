#include <windows.h>

#define HOTKEY_TOGGLE 1

bool aimAssist = false;
bool triggerBot = false;
bool safeMode = false;
HWND hwnd = NULL;
bool isMenuVisible = true;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_HOTKEY:
        if (wParam == HOTKEY_TOGGLE) {
            isMenuVisible = !isMenuVisible;
            ShowWindow(hWnd, isMenuVisible ? SW_SHOW : SW_HIDE);
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) aimAssist = !aimAssist;
        if (LOWORD(wParam) == 2) triggerBot = !triggerBot;
        if (LOWORD(wParam) == 3) safeMode = !safeMode;
        if (LOWORD(wParam) == 4) {
            // Panic: скрываем меню
            isMenuVisible = false;
            ShowWindow(hWnd, SW_HIDE);
            MessageBoxW(hWnd, L"Panic Mode Activated. Menu Hidden.", L"Ghost", MB_OK | MB_ICONWARNING);
        }
        break;
    case WM_CLOSE:
        isMenuVisible = false;
        ShowWindow(hWnd, SW_HIDE);
        return 0;
    case WM_DESTROY:
        UnregisterHotKey(hWnd, HOTKEY_TOGGLE);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI MenuThread(LPVOID) {
    const wchar_t CLASS_NAME[] = L"GhostMenuClass";
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(25, 25, 25));
    RegisterClassW(&wc);

    hwnd = CreateWindowExW(WS_EX_TOPMOST, CLASS_NAME, L"GHOST CLIENT | 1.21.11",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 350, 400,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    // Регистрируем хоткей на Правый Shift
    RegisterHotKey(hwnd, HOTKEY_TOGGLE, 0, VK_RSHIFT);

    CreateWindowW(L"BUTTON", L"Trigger Bot", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 20, 200, 30, hwnd, (HMENU)1, NULL, NULL);
    CreateWindowW(L"BUTTON", L"Aim Assist", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 60, 200, 30, hwnd, (HMENU)2, NULL, NULL);
    CreateWindowW(L"BUTTON", L"Safe Mode (Mirror)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 100, 200, 30, hwnd, (HMENU)3, NULL, NULL);
    CreateWindowW(L"STATIC", L"---------------------------------", WS_CHILD | WS_VISIBLE, 20, 140, 280, 20, hwnd, NULL, NULL, NULL);
    CreateWindowW(L"BUTTON", L"PANIC MODE (Hide)", WS_CHILD | WS_VISIBLE, 20, 170, 280, 40, hwnd, (HMENU)4, NULL, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MenuThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
