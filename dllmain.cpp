#include "pch.h"
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include "MinHook.h"
#include "BassProxy.h"
#include "GameEngine.h"
#include "UniversalObjects.h"
#include "Hooks.h"
#include "HamsterballAPI.h"
#include "ModAPI.h"
#include <filesystem>

namespace fs = std::filesystem;

extern void ReloadINI();
ModAPI g_ModApiInstance; 
void loadMods();

// Fullfilling Extern Promises
void* g_StolenPlayer = nullptr;
bool g_CheatSpeed = false;
bool g_CheatJump = false;
bool g_CheatNoBreak = false;
bool g_CheatTopDown = false;

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
HudManagerFunc Original_HudManager = nullptr;
RenderApplyFunc Original_RenderApply = nullptr;
BaseCollideCheckFunc Original_BaseCollideCheck = nullptr;
GeometryBinderFunc Original_BindGeometry = nullptr;
MasterLevelSetupFunc Original_MasterLevelSetup = nullptr;
RenderDynamic_t Original_RenderDynamic = nullptr;
Shatter1_t Original_Shatter1 = nullptr;
Shatter2_t Original_Shatter2 = nullptr;
Shatter3_t Original_Shatter3 = nullptr;

PollInputsFunc Original_PollInputs = nullptr;


// The Mod Thread
DWORD WINAPI ModThread(HMODULE hModule) {

    ReloadINI();
    loadMods(); 

    if (g_ShowConsole) {
        // Spawn command prompt window
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        printf("Hamsterball Mod Debug Console\n");
    }

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
    LPVOID hudManagerAddr = (LPVOID)(baseAddr + 0x1b710);
    LPVOID renderApplyAddr = (LPVOID)(baseAddr + 0x54A30);
    LPVOID baseCollideCheckAddr = (LPVOID)(baseAddr + 0xc5d0);
    Original_BindGeometry = (GeometryBinderFunc)(baseAddr + 0x602f0);
    LPVOID masterLevelSetupAddr = (LPVOID)(baseAddr + 0x1c5b0);
    //LPVOID renderDynamicAddr = (LPVOID)(baseAddr + 0xb570);
    LPVOID shatterHamsterAddr = (LPVOID)(baseAddr + 0x8d70);
    LPVOID pollInputsAddr = (LPVOID)(baseAddr + 0x6EBD0);

    
    MH_Initialize();

    MH_CreateHook(updateFuncAddr, &Hooked_PlayerUpdate, (LPVOID*)&Original_PlayerUpdate);
    MH_EnableHook(updateFuncAddr);

    MH_CreateHook(optionsFuncAddr, &Hooked_OptionsMenu, (LPVOID*)&Original_OptionsMenu);
    MH_EnableHook(optionsFuncAddr);

    MH_CreateHook(clickFuncAddr, &Hooked_OptionsClick, (LPVOID*)&Original_OptionsClick);
    MH_EnableHook(clickFuncAddr);

    MH_CreateHook(addMenuButtonAddr, &Hooked_AddMenuButton, (LPVOID*)&Original_AddMenuButton);
    MH_EnableHook(addMenuButtonAddr);

    MH_CreateHook(getLevelNameFuncAddr, &Hooked_GetLevelName, (LPVOID*)&Original_GetLevelName);
    MH_EnableHook(getLevelNameFuncAddr);

    MH_CreateHook(drawHUDTextAddr, &Hooked_DrawHUDText, (LPVOID*)&Original_DrawHUDText);
    MH_EnableHook(drawHUDTextAddr);

    MH_CreateHook(createColorAddr, &Hooked_CreateColor, (LPVOID*)&Original_CreateColor);
    MH_EnableHook(createColorAddr);

    MH_CreateHook(hudManagerAddr, &Hooked_HudManager, (LPVOID*)&Original_HudManager);
    MH_EnableHook(hudManagerAddr);

    MH_CreateHook(renderApplyAddr, &Hooked_RenderApply, (LPVOID*)&Original_RenderApply);
    MH_EnableHook(renderApplyAddr);

    MH_CreateHook(baseCollideCheckAddr, &Hooked_BaseCollideCheck, (LPVOID*)&Original_BaseCollideCheck);
    MH_EnableHook(baseCollideCheckAddr);

    MH_CreateHook(masterLevelSetupAddr, &Hooked_MasterLevelSetup, (LPVOID*)&Original_MasterLevelSetup);
    MH_EnableHook(masterLevelSetupAddr);

    //MH_CreateHook(renderDynamicAddr, &Hooked_RenderDynamic, (LPVOID*)&Original_RenderDynamic);
    //MH_EnableHook(renderDynamicAddr);

    LPVOID shatter1Addr = (LPVOID)(baseAddr + 0x8D70);
    MH_CreateHook(shatter1Addr, &Hooked_Shatter1, (LPVOID*)&Original_Shatter1);
    MH_EnableHook(shatter1Addr);
    LPVOID shatter2Addr = (LPVOID)(baseAddr + 0x9050);
    MH_CreateHook(shatter2Addr, &Hooked_Shatter2, (LPVOID*)&Original_Shatter2);
    MH_EnableHook(shatter2Addr);
    LPVOID shatter3Addr = (LPVOID)(baseAddr + 0x9480);
    MH_CreateHook(shatter3Addr, &Hooked_Shatter3, (LPVOID*)&Original_Shatter3);
    MH_EnableHook(shatter3Addr);

    MH_CreateHook(pollInputsAddr, &Hooked_PollInputs, (LPVOID*)&Original_PollInputs);
    MH_EnableHook(pollInputsAddr);

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

        //if (g_CheatJump) {
        //    bool isJumpKeyPressed = (GetAsyncKeyState(VK_LSHIFT) & 0x8000);
        //    if (isJumpKeyPressed && !wasJumpKeyPressed) {

        //        if (g_StolenPlayer != nullptr) {

        //            // Get the player's physics object
        //            DWORD* physicsObjPtr = (DWORD*)((DWORD)g_StolenPlayer + 0x1a4);

        //            if (physicsObjPtr != nullptr && *physicsObjPtr != 0) {

        //                DWORD physicsObj = *physicsObjPtr;

        //                // Read Y Velocity
        //                float* trueVelY = (float*)(physicsObj + 0xca8);

        //                // Ground Check
        //                float tolerance = 0.5f;

        //                if (*trueVelY > -tolerance && *trueVelY < tolerance) {

        //                    // Apply the jump force!
        //                    *trueVelY = 20.0f;
        //                }
        //            }
        //        }
        //    }
        //    wasJumpKeyPressed = isJumpKeyPressed;
        //}

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

void loadMods() {
    std::string modFolderPath = "Mods";

    for (const auto& entry : fs::directory_iterator(modFolderPath)) {
        // ignore non mods 
        if (entry.path().extension() == ".dll") {
            HMODULE modDLL = LoadLibraryA(entry.path().string().c_str());
            if (modDLL) {
                CreateModFunct factory = (CreateModFunct)GetProcAddress(modDLL, "CreateModInstance");
                if (factory) {
                    HamsterballAPI* newMod = factory();
                    if (newMod) {
                        newMod->Initialize(&g_ModApiInstance); 
                        g_Mods.push_back(newMod);
                    }
                }
            }
        }
    }
}