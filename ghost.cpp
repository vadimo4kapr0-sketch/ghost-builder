#include <windows.h>
#include <GL/gl.h>
#include "minhook-master/include/MinHook.h"

bool isHooked = false;
typedef BOOL(WINAPI* SwapBuffers_t)(HDC);
SwapBuffers_t oSwapBuffers = nullptr;

BOOL WINAPI hkSwapBuffers(HDC hdc) {
    if (GetAsyncKeyState(VK_RSHIFT) & 1) {
        // Если нажали Right Shift — красим экран в красный (тест хука)
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    return oSwapBuffers(hdc);
}

DWORD WINAPI MainThread(LPVOID) {
    while (!GetModuleHandleA("gdi32.dll")) Sleep(100);
    if (MH_Initialize() != MH_OK) return 1;

    void* target = (void*)GetProcAddress(GetModuleHandleA("gdi32.dll"), "SwapBuffers");
    if (MH_CreateHook(target, (LPVOID)hkSwapBuffers, reinterpret_cast<LPVOID*>(&oSwapBuffers)) == MH_OK) {
        MH_EnableHook(target);
        isHooked = true;
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
