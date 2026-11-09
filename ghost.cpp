#include <windows.h>
#include <GL/gl.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "minhook-master/include/MinHook.h"

bool showMenu = true;
bool triggerBot = false;
bool aimAssist = false;

typedef BOOL(WINAPI* SwapBuffers_t)(HDC);
SwapBuffers_t oSwapBuffers = nullptr;

BOOL WINAPI hkSwapBuffers(HDC hdc) {
    static bool init = false;
    if (!init) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;
        ImGui::StyleColorsDark();
        
        ImGui_ImplWin32_Init(WindowFromDC(hdc));
        ImGui_ImplOpenGL3_Init("#version 150");
        init = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_RSHIFT) & 1) {
        showMenu = !showMenu;
    }

    if (showMenu) {
        ImGui::Begin("GHOST CLIENT | 1.21.11", &showMenu);
        ImGui::Checkbox("Trigger Bot", &triggerBot);
        ImGui::Checkbox("Aim Assist", &aimAssist);
        ImGui::Separator();
        if (ImGui::Button("PANIC MODE (Hide)")) {
            showMenu = false;
        }
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return oSwapBuffers(hdc);
}

DWORD WINAPI MainThread(LPVOID) {
    while (!GetModuleHandleA("gdi32.dll")) Sleep(100);
    
    if (MH_Initialize() != MH_OK) return 1;

    // Хукаем SwapBuffers из gdi32.dll (его вызывает Java)
    void* target = (void*)GetProcAddress(GetModuleHandleA("gdi32.dll"), "SwapBuffers");
    
    if (MH_CreateHook(target, (LPVOID)hkSwapBuffers, reinterpret_cast<LPVOID*>(&oSwapBuffers)) == MH_OK) {
        MH_EnableHook(target);
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
