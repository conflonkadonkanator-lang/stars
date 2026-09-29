#include <windows.h>

#define ADDR_CURRENT_STARS 0x5591B7
#define ADDR_STAR_VISUAL    0x5591A6
#define ADDR_TARGET_STARS   0x6910D8
#define ADDR_GAME_PAUSE     0x869668

void WriteMemory(DWORD address, const void* value, int size) {
    DWORD oldProtect;
    VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((LPVOID)address, value, size);
    VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
}

template <typename T>
T ReadMemory(DWORD address) {
    return *(T*)address;
}

DWORD WINAPI ScriptThread(LPVOID lpParam) {
    while (ReadMemory<DWORD>(0x6644BC) == 0) {
        Sleep(100);
    }

    while (true) {
        Sleep(10); 

        unsigned char currentStars = ReadMemory<unsigned char>(ADDR_CURRENT_STARS);
        unsigned int starVisual    = ReadMemory<unsigned int>(ADDR_STAR_VISUAL);
        unsigned char targetStars  = ReadMemory<unsigned char>(ADDR_TARGET_STARS);
        bool isPaused              = (ReadMemory<unsigned char>(ADDR_GAME_PAUSE) != 0);

        int visualStyle = GetPrivateProfileIntA("VC.DynamicStars", "VisualStyle", 0, ".\\CLEO\\VC.DynamicStars.ini");
        unsigned char var0 = 0;

        if (currentStars != var0 && visualStyle == 0 && !isPaused) {
            WriteMemory(ADDR_CURRENT_STARS, &var0, 1);
        }
        if (currentStars != 6 && visualStyle == 1) {
            unsigned char val6 = 6;
            WriteMemory(ADDR_CURRENT_STARS, &val6, 1);
        }
        if (currentStars != targetStars && visualStyle == 2) {
            WriteMemory(ADDR_TARGET_STARS, &targetStars, 1);
        }
        if (var0 < 1 && starVisual != 0x90 && !isPaused) {
            unsigned int val90 = 0x90;
            WriteMemory(ADDR_STAR_VISUAL, &val90, 4);
        }
        if (var0 > 0 && starVisual != 0xFF7F2888 && !isPaused) {
            unsigned int valColor = 0xFF7F2888;
            WriteMemory(ADDR_STAR_VISUAL, &valColor, 4);
        }
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, ScriptThread, NULL, 0, NULL);
    }
    return TRUE;
}
