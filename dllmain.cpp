#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include "MinHook.h" // Include the MinHook library

// --- YOUR EXPORTS HERE ---
#pragma comment(linker, "/export:BASS_Apply3D=bass_real.BASS_Apply3D")
#pragma comment(linker, "/export:BASS_ChannelBytes2Seconds=bass_real.BASS_ChannelBytes2Seconds")
#pragma comment(linker, "/export:BASS_ChannelGet3DAttributes=bass_real.BASS_ChannelGet3DAttributes")
#pragma comment(linker, "/export:BASS_ChannelGet3DPosition=bass_real.BASS_ChannelGet3DPosition")
#pragma comment(linker, "/export:BASS_ChannelGetAttributes=bass_real.BASS_ChannelGetAttributes")
#pragma comment(linker, "/export:BASS_ChannelGetData=bass_real.BASS_ChannelGetData")
#pragma comment(linker, "/export:BASS_ChannelGetDevice=bass_real.BASS_ChannelGetDevice")
#pragma comment(linker, "/export:BASS_ChannelGetEAXMix=bass_real.BASS_ChannelGetEAXMix")
#pragma comment(linker, "/export:BASS_ChannelGetInfo=bass_real.BASS_ChannelGetInfo")
#pragma comment(linker, "/export:BASS_ChannelGetLevel=bass_real.BASS_ChannelGetLevel")
#pragma comment(linker, "/export:BASS_ChannelGetPosition=bass_real.BASS_ChannelGetPosition")
#pragma comment(linker, "/export:BASS_ChannelIsActive=bass_real.BASS_ChannelIsActive")
#pragma comment(linker, "/export:BASS_ChannelIsSliding=bass_real.BASS_ChannelIsSliding")
#pragma comment(linker, "/export:BASS_ChannelPause=bass_real.BASS_ChannelPause")
#pragma comment(linker, "/export:BASS_ChannelRemoveDSP=bass_real.BASS_ChannelRemoveDSP")
#pragma comment(linker, "/export:BASS_ChannelRemoveFX=bass_real.BASS_ChannelRemoveFX")
#pragma comment(linker, "/export:BASS_ChannelRemoveLink=bass_real.BASS_ChannelRemoveLink")
#pragma comment(linker, "/export:BASS_ChannelRemoveSync=bass_real.BASS_ChannelRemoveSync")
#pragma comment(linker, "/export:BASS_ChannelResume=bass_real.BASS_ChannelResume")
#pragma comment(linker, "/export:BASS_ChannelSeconds2Bytes=bass_real.BASS_ChannelSeconds2Bytes")
#pragma comment(linker, "/export:BASS_ChannelSet3DAttributes=bass_real.BASS_ChannelSet3DAttributes")
#pragma comment(linker, "/export:BASS_ChannelSet3DPosition=bass_real.BASS_ChannelSet3DPosition")
#pragma comment(linker, "/export:BASS_ChannelSetAttributes=bass_real.BASS_ChannelSetAttributes")
#pragma comment(linker, "/export:BASS_ChannelSetDSP=bass_real.BASS_ChannelSetDSP")
#pragma comment(linker, "/export:BASS_ChannelSetEAXMix=bass_real.BASS_ChannelSetEAXMix")
#pragma comment(linker, "/export:BASS_ChannelSetFX=bass_real.BASS_ChannelSetFX")
#pragma comment(linker, "/export:BASS_ChannelSetLink=bass_real.BASS_ChannelSetLink")
#pragma comment(linker, "/export:BASS_ChannelSetPosition=bass_real.BASS_ChannelSetPosition")
#pragma comment(linker, "/export:BASS_ChannelSetSync=bass_real.BASS_ChannelSetSync")
#pragma comment(linker, "/export:BASS_ChannelSlideAttributes=bass_real.BASS_ChannelSlideAttributes")
#pragma comment(linker, "/export:BASS_ChannelStop=bass_real.BASS_ChannelStop")
#pragma comment(linker, "/export:BASS_ErrorGetCode=bass_real.BASS_ErrorGetCode")
#pragma comment(linker, "/export:BASS_FXGetParameters=bass_real.BASS_FXGetParameters")
#pragma comment(linker, "/export:BASS_FXSetParameters=bass_real.BASS_FXSetParameters")
#pragma comment(linker, "/export:BASS_Free=bass_real.BASS_Free")
#pragma comment(linker, "/export:BASS_Get3DFactors=bass_real.BASS_Get3DFactors")
#pragma comment(linker, "/export:BASS_Get3DPosition=bass_real.BASS_Get3DPosition")
#pragma comment(linker, "/export:BASS_GetCPU=bass_real.BASS_GetCPU")
#pragma comment(linker, "/export:BASS_GetConfig=bass_real.BASS_GetConfig")
#pragma comment(linker, "/export:BASS_GetDSoundObject=bass_real.BASS_GetDSoundObject")
#pragma comment(linker, "/export:BASS_GetDevice=bass_real.BASS_GetDevice")
#pragma comment(linker, "/export:BASS_GetDeviceDescription=bass_real.BASS_GetDeviceDescription")
#pragma comment(linker, "/export:BASS_GetEAXParameters=bass_real.BASS_GetEAXParameters")
#pragma comment(linker, "/export:BASS_GetInfo=bass_real.BASS_GetInfo")
#pragma comment(linker, "/export:BASS_GetVersion=bass_real.BASS_GetVersion")
#pragma comment(linker, "/export:BASS_GetVolume=bass_real.BASS_GetVolume")
#pragma comment(linker, "/export:BASS_Init=bass_real.BASS_Init")
#pragma comment(linker, "/export:BASS_MusicFree=bass_real.BASS_MusicFree")
#pragma comment(linker, "/export:BASS_MusicGetLength=bass_real.BASS_MusicGetLength")
#pragma comment(linker, "/export:BASS_MusicGetName=bass_real.BASS_MusicGetName")
#pragma comment(linker, "/export:BASS_MusicGetVolume=bass_real.BASS_MusicGetVolume")
#pragma comment(linker, "/export:BASS_MusicLoad=bass_real.BASS_MusicLoad")
#pragma comment(linker, "/export:BASS_MusicPlay=bass_real.BASS_MusicPlay")
#pragma comment(linker, "/export:BASS_MusicPlayEx=bass_real.BASS_MusicPlayEx")
#pragma comment(linker, "/export:BASS_MusicPreBuf=bass_real.BASS_MusicPreBuf")
#pragma comment(linker, "/export:BASS_MusicSetAmplify=bass_real.BASS_MusicSetAmplify")
#pragma comment(linker, "/export:BASS_MusicSetPanSep=bass_real.BASS_MusicSetPanSep")
#pragma comment(linker, "/export:BASS_MusicSetPositionScaler=bass_real.BASS_MusicSetPositionScaler")
#pragma comment(linker, "/export:BASS_MusicSetVolume=bass_real.BASS_MusicSetVolume")
#pragma comment(linker, "/export:BASS_Pause=bass_real.BASS_Pause")
#pragma comment(linker, "/export:BASS_RecordFree=bass_real.BASS_RecordFree")
#pragma comment(linker, "/export:BASS_RecordGetDevice=bass_real.BASS_RecordGetDevice")
#pragma comment(linker, "/export:BASS_RecordGetDeviceDescription=bass_real.BASS_RecordGetDeviceDescription")
#pragma comment(linker, "/export:BASS_RecordGetInfo=bass_real.BASS_RecordGetInfo")
#pragma comment(linker, "/export:BASS_RecordGetInput=bass_real.BASS_RecordGetInput")
#pragma comment(linker, "/export:BASS_RecordGetInputName=bass_real.BASS_RecordGetInputName")
#pragma comment(linker, "/export:BASS_RecordInit=bass_real.BASS_RecordInit")
#pragma comment(linker, "/export:BASS_RecordSetDevice=bass_real.BASS_RecordSetDevice")
#pragma comment(linker, "/export:BASS_RecordSetInput=bass_real.BASS_RecordSetInput")
#pragma comment(linker, "/export:BASS_RecordStart=bass_real.BASS_RecordStart")
#pragma comment(linker, "/export:BASS_SampleCreate=bass_real.BASS_SampleCreate")
#pragma comment(linker, "/export:BASS_SampleCreateDone=bass_real.BASS_SampleCreateDone")
#pragma comment(linker, "/export:BASS_SampleFree=bass_real.BASS_SampleFree")
#pragma comment(linker, "/export:BASS_SampleGetInfo=bass_real.BASS_SampleGetInfo")
#pragma comment(linker, "/export:BASS_SampleLoad=bass_real.BASS_SampleLoad")
#pragma comment(linker, "/export:BASS_SamplePlay=bass_real.BASS_SamplePlay")
#pragma comment(linker, "/export:BASS_SamplePlay3D=bass_real.BASS_SamplePlay3D")
#pragma comment(linker, "/export:BASS_SamplePlay3DEx=bass_real.BASS_SamplePlay3DEx")
#pragma comment(linker, "/export:BASS_SamplePlayEx=bass_real.BASS_SamplePlayEx")
#pragma comment(linker, "/export:BASS_SampleSetInfo=bass_real.BASS_SampleSetInfo")
#pragma comment(linker, "/export:BASS_SampleStop=bass_real.BASS_SampleStop")
#pragma comment(linker, "/export:BASS_Set3DFactors=bass_real.BASS_Set3DFactors")
#pragma comment(linker, "/export:BASS_Set3DPosition=bass_real.BASS_Set3DPosition")
#pragma comment(linker, "/export:BASS_SetConfig=bass_real.BASS_SetConfig")
#pragma comment(linker, "/export:BASS_SetDevice=bass_real.BASS_SetDevice")
#pragma comment(linker, "/export:BASS_SetEAXParameters=bass_real.BASS_SetEAXParameters")
#pragma comment(linker, "/export:BASS_SetVolume=bass_real.BASS_SetVolume")
#pragma comment(linker, "/export:BASS_Start=bass_real.BASS_Start")
#pragma comment(linker, "/export:BASS_Stop=bass_real.BASS_Stop")
#pragma comment(linker, "/export:BASS_StreamCreate=bass_real.BASS_StreamCreate")
#pragma comment(linker, "/export:BASS_StreamCreateFile=bass_real.BASS_StreamCreateFile")
#pragma comment(linker, "/export:BASS_StreamCreateFileUser=bass_real.BASS_StreamCreateFileUser")
#pragma comment(linker, "/export:BASS_StreamCreateURL=bass_real.BASS_StreamCreateURL")
#pragma comment(linker, "/export:BASS_StreamFree=bass_real.BASS_StreamFree")
#pragma comment(linker, "/export:BASS_StreamGetFilePosition=bass_real.BASS_StreamGetFilePosition")
#pragma comment(linker, "/export:BASS_StreamGetLength=bass_real.BASS_StreamGetLength")
#pragma comment(linker, "/export:BASS_StreamGetTags=bass_real.BASS_StreamGetTags")
#pragma comment(linker, "/export:BASS_StreamPlay=bass_real.BASS_StreamPlay")
#pragma comment(linker, "/export:BASS_StreamPreBuf=bass_real.BASS_StreamPreBuf")
#pragma comment(linker, "/export:BASS_Update=bass_real.BASS_Update")
#pragma comment(linker, "/export:_=bass_real._")


// The stolen player pointer
void* g_StolenPlayer = nullptr;

// Mod's global state
bool g_ModEnabled = true;

// Break/Die function (0x00405190)
typedef void(__fastcall* FindRespawnPointFunc)(void* ecx_playerObject, void* edx_dummy);
FindRespawnPointFunc Original_FindRespawn = nullptr;

// Player Update Loop (0x00405e00)
typedef void(__fastcall* PlayerUpdateFunc)(void* ecx_player, void* edx_dummy);
PlayerUpdateFunc Original_PlayerUpdate = nullptr;

// Options Menu function (0x00442ce0)
typedef void* (__fastcall* OptionsMenuFunc)(void* this_ptr, void* edx_dummy, int param_1, int param_2);
OptionsMenuFunc Original_OptionsMenu = nullptr;

// AddMenuButton function (0x004497f0)
typedef void(__fastcall* AddMenuButtonFunc)(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, const char* style, const char* unk);
AddMenuButtonFunc Original_AddMenuButton = nullptr;

// AddSpacer function (0x00449430)
typedef void(__fastcall* AddSpacerFunc)(void* this_ptr, void* edx_dummy, int height);
AddSpacerFunc Original_AddSpacer = nullptr;

// Options Click Handler (0x004434f0)
typedef void(__fastcall* OptionsClickFunc)(void* this_ptr, void* edx_dummy, const char* clicked_id);
OptionsClickFunc Original_OptionsClick = nullptr;

// Button Text Updater (0x0044a8b0)
typedef void(__fastcall* UpdateButtonTextFunc)(void* this_ptr, void* edx_dummy, const char* newText, const char* id);
UpdateButtonTextFunc Game_UpdateButtonText = nullptr;

// Hooked Player Update Loop (for getting player object pointer)
void __fastcall Hooked_PlayerUpdate(void* ecx_player, void* edx_dummy) {
    // Passively steal the player pointer 60 times a second
    g_StolenPlayer = ecx_player;

    // Resume normal game logic
    Original_PlayerUpdate(ecx_player, edx_dummy);
}

// Hooked Options Menu
void* __fastcall Hooked_OptionsMenu(void* this_ptr, void* edx_dummy, int param_1, int param_2) {

    // Call the original function, save the pointer that it returns
    void* menuPointer = Original_OptionsMenu(this_ptr, edx_dummy, param_1, param_2);

    DWORD baseAddr = (DWORD)GetModuleHandle(NULL);

    // Add spacer
    Original_AddSpacer(this_ptr, nullptr, 10);

    // Color Object vtable pointer
    DWORD vtableAddr = baseAddr + 0xCF300;

    // Add custom options button
    const char* defaultText = g_ModEnabled ? "MOD ENABLED: YES" : "MOD ENABLED: NO";
    Original_AddMenuButton(this_ptr, nullptr, defaultText, "MOD_TOGGLE", vtableAddr, 1.0f, 1.0f, 1.0f, "j", nullptr);

    // Return the saved pointer
    return menuPointer;
}

void __fastcall Hooked_OptionsClick(void* this_ptr, void* edx_dummy, const char* clicked_id) {

    // Check if the player clicked our custom button
    if (strcmp(clicked_id, "MOD_TOGGLE") == 0) {

        // Flip state bool
        g_ModEnabled = !g_ModEnabled;

        // Change text
        const char* newText = g_ModEnabled ? "MOD ENABLED: YES" : "MOD ENABLED: NO";

        // Redraw button
        Game_UpdateButtonText(this_ptr, nullptr, newText, "MOD_TOGGLE");

        // Return immediately
        return;
    }

    // New resolution button code to add new resolutions
    if (strcmp(clicked_id, "REZ") == 0) {

        // Get direct pointers to the width and height stored in the menu object
        int* currentWidth = (int*)((DWORD)this_ptr + 0xCE4);
        int* currentHeight = (int*)((DWORD)this_ptr + 0xCE8);

        // Custom resolution cycle
        if (*currentWidth == 800 && *currentHeight == 600) {
            *currentWidth = 1024; *currentHeight = 768;
        }
        else if (*currentWidth == 1024 && *currentHeight == 768) {
            *currentWidth = 1280; *currentHeight = 720; // Added 720p
        }
        else if (*currentWidth == 1280 && *currentHeight == 720) {
            *currentWidth = 1920; *currentHeight = 1080; // Added 1080p
        }
        else {
            // If it's 1080p (or anything else), loop back to 800x600
            *currentWidth = 800; *currentHeight = 600;
        }

        // Format button text
        char newResText[256];
        sprintf_s(newResText, "RESOLUTION: %d X %d", *currentWidth, *currentHeight);

        // Redraw button
        Game_UpdateButtonText(this_ptr, nullptr, newResText, "REZ");

        // Return immediately
        return;
    }

    // If it was any other button, pass it to the original game logic
    Original_OptionsClick(this_ptr, edx_dummy, clicked_id);
}

// The Mod Thread
DWORD WINAPI ModThread(HMODULE hModule) {

    // Defeat ASLR
    DWORD baseAddr = (DWORD)GetModuleHandle(NULL);

    // The exact function addresses
    LPVOID updateFuncAddr = (LPVOID)(baseAddr + 0x5E00);
    Original_FindRespawn = (FindRespawnPointFunc)(baseAddr + 0x5190);

    LPVOID optionsFuncAddr = (LPVOID)(baseAddr + 0x42ce0);
    Original_AddMenuButton = (AddMenuButtonFunc)(baseAddr + 0x497f0);
    Original_AddSpacer = (AddSpacerFunc)(baseAddr + 0x49430);
    LPVOID clickFuncAddr = (LPVOID)(baseAddr + 0x434F0);
    Game_UpdateButtonText = (UpdateButtonTextFunc)(baseAddr + 0x4a8b0);

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

    // Main Hotkey Loop
    while (true) {
        if (GetAsyncKeyState('X') & 0x8000) {

            if (g_StolenPlayer != nullptr) {
                // Call the original death function using our passively stolen pointer
                Original_FindRespawn(g_StolenPlayer, nullptr);
            }

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