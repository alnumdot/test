#include "headers.h"

void main() {
    g_module = (uintptr_t)GetModuleHandleA("RainbowSix.exe");
    std::thread cache_thread(cache.setup_cache);
    cache_thread.detach();
    
    while (true) {
        for (auto& actor : cache.actor_list) {
            //if (actor->is_actor_player()) {
            //    auto pos = actor->get_actor_position();
            //    printf("Position: X=%.2f, Y=%.2f, Z=%.2f\n", pos.x, pos.y, pos.z);
            //}
        }
        Sleep(50);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) main();
    return TRUE;
}