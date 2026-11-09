#include <windows.h>

extern "C" __declspec(dllexport) void Dummy() {}

BOOL APIENTRY DllMain(HMODULE h, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(h);
        MessageBoxW(nullptr, L"ghost.dll injected", L"ghost", MB_OK | MB_ICONINFORMATION);
    }
    return TRUE;
}
