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
#include <fstream>
#include "InitHelpers.h"
#include "CoreFeatures.h"

namespace fs = std::filesystem;

ModAPI g_ModApiInstance; 
void loadMods();

// Fullfilling Extern Promises
Ball* g_Player = nullptr;
Ball* g_Player2 = nullptr;
Ball* g_Player3 = nullptr;
Ball* g_Player4 = nullptr;
int* g_Timer = nullptr;

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
//BaseCollideCheckFunc Original_BaseCollideCheck = nullptr;
GeometryBinderFunc Original_BindGeometry = nullptr;
MasterLevelSetupFunc Original_MasterLevelSetup = nullptr;
RenderDynamic_t Original_RenderDynamic = nullptr;
SaveConfigFunc Original_SaveConfig = nullptr; 
PollInputsFunc Original_PollInputs = nullptr;
GameUpdateFunc Original_GameUpdate = nullptr;
ApplyForceFunc ApplyForce = nullptr;
CollisionCheckFunc Original_CollisionCheck = nullptr; 
SliderOptionHandlerFunc Original_SliderOptionHandler = nullptr;
LevelRaycastFunc LevelRaycast = nullptr;
PlaySoundFunc PlaySoundEffect = nullptr;
Play3dSoundFunc Play3dSoundEffect = nullptr;
ShowBallMessageFunc ShowBallMessage = nullptr;
RenderTextLoopFunc Original_RenderTextLoop = nullptr;
DrawTextNoShadowFunc DrawTextNoShadow = nullptr;
DrawGameTextFunc DrawGameText = nullptr;

App* g_App = nullptr;
PhysicsConstants* g_PhysicsConstants = nullptr;

// The Mod Thread
DWORD WINAPI ModThread(HMODULE hModule) {

    InitDevConsole(); 
    InitResolutions(); 

    DWORD baseAddr = (DWORD)GetModuleHandle(NULL);

    g_App = (App*)(baseAddr + 0xFD680);
    g_Timer = (int*)(baseAddr + 0xFDC68);
    g_PhysicsConstants = (PhysicsConstants*)(baseAddr + 0xCF368);
    ApplyForce = (ApplyForceFunc)(baseAddr + 0x02650);
    LevelRaycast = (LevelRaycastFunc)(baseAddr + 0x65D90);
    PlaySoundEffect = (PlaySoundFunc)(baseAddr + 0x597b0);
    Play3dSoundEffect = (Play3dSoundFunc)(baseAddr + 0x59860);
    ShowBallMessage = (ShowBallMessageFunc)(baseAddr + 0x01660); 
    DrawTextNoShadow = (DrawTextNoShadowFunc)(baseAddr + 0x2C870);
    DrawGameText = (DrawGameTextFunc)(baseAddr + 0x09C60);
    DWORD oldProtect; // unlock memory for constants (causes crashes without if you try to edit the physics constants)
    if (!VirtualProtect(g_PhysicsConstants, sizeof(PhysicsConstants), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("ERROR: FAILED TO UNLOCK PHYSICS CONSTANT MEMORY");
    }

    MH_Initialize();
    loadMods();
    ReloadINI();

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
    //LPVOID baseCollideCheckAddr = (LPVOID)(baseAddr + 0xc5d0);
    Original_BindGeometry = (GeometryBinderFunc)(baseAddr + 0x602f0);
    LPVOID masterLevelSetupAddr = (LPVOID)(baseAddr + 0x1c5b0);
    //LPVOID renderDynamicAddr = (LPVOID)(baseAddr + 0xb570);
    LPVOID pollInputsAddr = (LPVOID)(baseAddr + 0x6EBD0);
    LPVOID gameUpdateAddr = (LPVOID)(baseAddr + 0x6C170);
    LPVOID collisionCheckAddr = (LPVOID)(baseAddr + 0x0C5D0);
    LPVOID sliderOptionHandlerAddr = (LPVOID)(baseAddr + 0x42680);
    LPVOID saveConfigAddr = (LPVOID)(baseAddr + 0x284c0);
    LPVOID renderTextLoopAddr = (LPVOID)(baseAddr + 0x6C250);

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

    //MH_CreateHook(baseCollideCheckAddr, &Hooked_BaseCollideCheck, (LPVOID*)&Original_BaseCollideCheck);
    //MH_EnableHook(baseCollideCheckAddr);

    MH_CreateHook(masterLevelSetupAddr, &Hooked_MasterLevelSetup, (LPVOID*)&Original_MasterLevelSetup);
    MH_EnableHook(masterLevelSetupAddr);

    //MH_CreateHook(renderDynamicAddr, &Hooked_RenderDynamic, (LPVOID*)&Original_RenderDynamic);
    //MH_EnableHook(renderDynamicAddr);

    MH_CreateHook(pollInputsAddr, &Hooked_PollInputs, (LPVOID*)&Original_PollInputs);
    MH_EnableHook(pollInputsAddr);

    MH_CreateHook(gameUpdateAddr, &Hooked_GameUpdate, (LPVOID*)&Original_GameUpdate);
    MH_EnableHook(gameUpdateAddr);

    MH_CreateHook(collisionCheckAddr, &Hooked_CollisionCheck, (LPVOID*)&Original_CollisionCheck);
    MH_EnableHook(collisionCheckAddr);

    MH_CreateHook(sliderOptionHandlerAddr, &Hooked_SliderOptionHandler, (LPVOID*)&Original_SliderOptionHandler);
    MH_EnableHook(sliderOptionHandlerAddr);

    MH_CreateHook(saveConfigAddr, &Hooked_SaveConfig, (LPVOID*)&Original_SaveConfig);
    MH_EnableHook(saveConfigAddr); 

    MH_CreateHook(renderTextLoopAddr, &Hooked_RenderTextLoop, (LPVOID*)&Original_RenderTextLoop);
    MH_EnableHook(renderTextLoopAddr);


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
    HamsterballAPI* coreMod = new CoreFeatures(); 
    coreMod->Initialize(&g_ModApiInstance);
    g_Mods.push_back(coreMod); 

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
                        if (g_ShowConsole) printf("Mod Loaded: %s, Author: %s, API Version: %d\n", newMod->GetModName(), newMod->GetAuthorName(), newMod->GetApiVersion());
                        newMod->Initialize(&g_ModApiInstance); 
                        g_Mods.push_back(newMod);
                    }
                }
            }
        }
    }
}
