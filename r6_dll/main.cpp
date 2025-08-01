#include "headers.h"

void allocate_console() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    HWND hConsole = GetConsoleWindow();
    MoveWindow(hConsole, 100, 100, 800, 600, TRUE);
}


void main() {
    allocate_console();
    
    g_module = (uintptr_t)GetModuleHandleA("RainbowSix.exe");
    std::thread cache_thread(cache.setup_cache);
    cache_thread.detach();
    
    printf("start\nmod base: 0x%llx\n", g_module);

    while (true) {
        if (!cache.camera) continue;

        for (auto& actor : cache.actor_list) {

            if (!actor) continue;

            if (actor->is_actor_player()) {
                auto pos = actor->get_actor_position();
                printf("Position: X=%.2f, Y=%.2f, Z=%.2f\n", pos.x, pos.y, pos.z);
            }
        }
        Sleep(50);
    }
    printf("end\n");
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(main), NULL, 0, NULL);
    }
    return TRUE;
}