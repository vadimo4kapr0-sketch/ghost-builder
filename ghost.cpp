#include <windows.h>

DWORD WINAPI TestThread(LPVOID) {
    MessageBoxW(NULL, L"Manual Mapping Works!", L"Test", MB_OK);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, TestThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
