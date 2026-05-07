#include "pch.h"
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include "MinHook.h"
#include "BassProxy.h"
#include "GameEngine.h"
#include "Hooks.h"

extern void ReloadINI();

// Fullfilling Extern Promises
void* g_StolenPlayer = nullptr;
bool g_SpeedUncap = false;

FindRespawnPointFunc Original_FindRespawn = nullptr;
PlayerUpdateFunc Original_PlayerUpdate = nullptr;
OptionsMenuFunc Original_OptionsMenu = nullptr;
AddMenuButtonFunc Original_AddMenuButton = nullptr;
AddSpacerFunc Original_AddSpacer = nullptr;
OptionsClickFunc Original_OptionsClick = nullptr;
UpdateButtonTextFunc Game_UpdateButtonText = nullptr;
GetLevelNameFunc Original_GetLevelName = nullptr;
DrawHUDTextFunc Original_DrawHUDText = nullptr;
CreateColorFunc Original_CreateColor = nullptr;

// The Mod Thread
DWORD WINAPI ModThread(HMODULE hModule) {

    // Spawn command prompt window
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("Hamsterball Mod Debug Console\n");

    // Get base address
    DWORD baseAddr = (DWORD)GetModuleHandle(NULL);

    // The exact function addresses
    LPVOID updateFuncAddr = (LPVOID)(baseAddr + 0x5E00);
    Original_FindRespawn = (FindRespawnPointFunc)(baseAddr + 0x5190);
    LPVOID optionsFuncAddr = (LPVOID)(baseAddr + 0x42ce0);
    LPVOID addMenuButtonAddr = (LPVOID)(baseAddr + 0x497f0);
    Original_AddSpacer = (AddSpacerFunc)(baseAddr + 0x49430);
    LPVOID clickFuncAddr = (LPVOID)(baseAddr + 0x434F0);
    Game_UpdateButtonText = (UpdateButtonTextFunc)(baseAddr + 0x4a8b0);
    LPVOID getLevelNameFuncAddr = (GetLevelNameFunc)(baseAddr + 0x264a0);
    LPVOID drawHUDTextAddr = (LPVOID)(baseAddr + 0x13a0);
    LPVOID createColorAddr = (LPVOID)(baseAddr + 0x53150);

    
    MH_Initialize();

    // Hook the main update loop
    MH_CreateHook(updateFuncAddr, &Hooked_PlayerUpdate, (LPVOID*)&Original_PlayerUpdate);
    MH_EnableHook(updateFuncAddr);

    // Hook the options menu function
    MH_CreateHook(optionsFuncAddr, &Hooked_OptionsMenu, (LPVOID*)&Original_OptionsMenu);
    MH_EnableHook(optionsFuncAddr);

    // Hook the options click handler
    MH_CreateHook(clickFuncAddr, &Hooked_OptionsClick, (LPVOID*)&Original_OptionsClick);
    MH_EnableHook(clickFuncAddr);

    // Hook the button creator
    MH_CreateHook(addMenuButtonAddr, &Hooked_AddMenuButton, (LPVOID*)&Original_AddMenuButton);
    MH_EnableHook(addMenuButtonAddr);

    // Hook the level name getter
    MH_CreateHook(getLevelNameFuncAddr, &Hooked_GetLevelName, (LPVOID*)&Original_GetLevelName);
    MH_EnableHook(getLevelNameFuncAddr);

    // Hook the hud text drawer
    MH_CreateHook(drawHUDTextAddr, &Hooked_DrawHUDText, (LPVOID*)&Original_DrawHUDText);
    MH_EnableHook(drawHUDTextAddr);

    // Hook the color creator
    MH_CreateHook(createColorAddr, &Hooked_CreateColor, (LPVOID*)&Original_CreateColor);
    MH_EnableHook(createColorAddr);

    ReloadINI();

    bool wasJumpKeyPressed = false;

    // Main Hotkey Loop
    while (true) {
        if (GetAsyncKeyState('X') & 0x8000) {

            if (g_StolenPlayer != nullptr) {
                // Call the original death function using our passively stolen pointer
                Original_FindRespawn(g_StolenPlayer, nullptr);
            }
            Sleep(500);
        }

        bool isJumpKeyPressed = (GetAsyncKeyState(VK_LSHIFT) & 0x8000);
        if (isJumpKeyPressed && !wasJumpKeyPressed) {

            if (g_StolenPlayer != nullptr) {

                // Get the player's physics object
                DWORD* physicsObjPtr = (DWORD*)((DWORD)g_StolenPlayer + 0x1a4);

                if (physicsObjPtr != nullptr && *physicsObjPtr != 0) {

                    DWORD physicsObj = *physicsObjPtr;

                    // Read Y Velocity
                    float* trueVelY = (float*)(physicsObj + 0xca8);

                    // Ground Check
                    float tolerance = 0.5f;

                    if (*trueVelY > -tolerance && *trueVelY < tolerance) {

                        // Apply the jump force!
                        *trueVelY = 20.0f;
                    }
                }
            }
        }
        wasJumpKeyPressed = isJumpKeyPressed;

        if (GetAsyncKeyState(VK_F5) & 0x8000) {
            ReloadINI();
            Sleep(500);
        }

        Sleep(10);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ModThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        // Clean up MinHook when the game closes
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}