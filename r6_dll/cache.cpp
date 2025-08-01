#include "headers.h"

// this namespace was generated from my dumper which gathers and auto generates shellcode for each build
// I will provide my current build for the latest r6s update (25 June 2025)
// you can go into IDA and find these addresses and make your own sigs for your build...

namespace offset
{
    inline constexpr unsigned long long camera_patch_address = 0xA8E727B;
    inline constexpr unsigned long long actor_patch_address = 0xA55325A;
    inline constexpr unsigned long long code_cave_one = 0x113DA35E;
    inline constexpr unsigned long long code_cave_two = 0x114211D4;
    inline constexpr unsigned char actor_mov_instruction[3] = { 0x4C, 0x89, 0x2D };
};


DWORD GetProcessIdByName(const std::wstring& processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe))
    {
        do
        {
            if (wcscmp(pe.szExeFile, processName.c_str()) == 0)
            {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

void c_cache::setup_camera()
{
    const auto patch_address = g_module + offset::camera_patch_address;
    const auto codecave_address = g_module + offset::code_cave_one;

    int rel_offset = static_cast<int>(codecave_address - (patch_address + 7));

    unsigned char patch[8] = {
        0x48, 0x89, 0x15,
        static_cast<unsigned char>(rel_offset & 0xFF),
        static_cast<unsigned char>((rel_offset >> 8) & 0xFF),
        static_cast<unsigned char>((rel_offset >> 16) & 0xFF),
        static_cast<unsigned char>((rel_offset >> 24) & 0xFF),
        0x90
    };

    DWORD processId = GetProcessIdByName(L"RainbowSix.exe");
    if (!processId) {
        std::cout << "Process not found" << std::endl;
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE, processId);
    if (!hProcess) {
        std::cout << "Failed to open process: " << GetLastError() << std::endl;
        return;
    }

    DWORD old_protect;
    if (!VirtualProtectEx(hProcess, (LPVOID)patch_address, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_protect)) {
        std::cout << "VirtualProtectEx failed: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, (LPVOID)patch_address, patch, sizeof(patch), &bytesWritten)) {
        std::cout << "WriteProcessMemory failed: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return;
    }

    VirtualProtectEx(hProcess, (LPVOID)patch_address, sizeof(patch), old_protect, &old_protect);

    CloseHandle(hProcess);

    //DWORD old_protect;
    //VirtualProtect((LPVOID)patch_address, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_protect);
    //if (!VirtualProtect((LPVOID)patch_address, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_protect)) {
    //    std::cout << "VirtualProtect failed: " << GetLastError() << std::endl;
    //    return;
    //}
    //memcpy((void*)patch_address, patch, sizeof(patch));
    //VirtualProtect((LPVOID)patch_address, sizeof(patch), old_protect, &old_protect);
}

void c_cache::setup_actor()
{
    const auto patch_address = g_module + offset::actor_patch_address;
    const auto codecave_address = g_module + offset::code_cave_two;

    int rel_offset = static_cast<int>(codecave_address - (patch_address + 7));

    unsigned char patch[12] = {
        offset::actor_mov_instruction[0],  offset::actor_mov_instruction[1],  offset::actor_mov_instruction[2],
        static_cast<unsigned char>(rel_offset & 0xFF),
        static_cast<unsigned char>((rel_offset >> 8) & 0xFF),
        static_cast<unsigned char>((rel_offset >> 16) & 0xFF),
        static_cast<unsigned char>((rel_offset >> 24) & 0xFF),
        0x90, 0x90, 0x90, 0x90, 0x90
    };

    DWORD processId = GetProcessIdByName(L"RainbowSix.exe");
    if (!processId) {
        std::cout << "Process not found" << std::endl;
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE, processId);
    if (!hProcess) {
        std::cout << "Failed to open process: " << GetLastError() << std::endl;
        return;
    }

    DWORD old_protect;
    if (!VirtualProtectEx(hProcess, (LPVOID)patch_address, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_protect)) {
        std::cout << "VirtualProtectEx failed: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, (LPVOID)patch_address, patch, sizeof(patch), &bytesWritten)) {
        std::cout << "WriteProcessMemory failed: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return;
    }

    VirtualProtectEx(hProcess, (LPVOID)patch_address, sizeof(patch), old_protect, &old_protect);

    CloseHandle(hProcess);

    //DWORD old_protect;
    //VirtualProtect((LPVOID)patch_address, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_protect);
    //if (!VirtualProtect((LPVOID)patch_address, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_protect)) {
    //    std::cout << "VirtualProtect failed: " << GetLastError() << std::endl;
    //    return;
    //}
    //memcpy((void*)patch_address, patch, sizeof(patch));
    //VirtualProtect((LPVOID)patch_address, sizeof(patch), old_protect, &old_protect);
}

void c_cache::setup_cache()
{
    setup_camera();
    setup_actor();

    std::vector<c_actor*> temp_list;

    // we dont sleep in this loop due to the fact that we are moving the register into a codecave, the function for actor gets called very fast and pointers changes alot. 
    // If we sleep we may miss an actor pointer leading to it not drawing all players

    while (true)
    {
        unsigned long long camera_addr = *(unsigned long long*)(g_module + offset::code_cave_one);
        if (!camera_addr)
            continue;

        camera = reinterpret_cast<c_camera*>(camera_addr);

        unsigned long long entity_addr = *(unsigned long long*)(g_module + offset::code_cave_two);
        if (!entity_addr)
            continue;

        c_actor* actor = reinterpret_cast<c_actor*>(entity_addr);

        if (std::find(temp_list.begin(), temp_list.end(), actor) != temp_list.end())
            continue;

        temp_list.push_back(actor);
        actor_list = temp_list;
    }
}