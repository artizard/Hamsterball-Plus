// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

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




DWORD WINAPI ModThread(HMODULE hModule) {
    // This loop runs endlessly in the background
    while (true) {

        // Check if the 'K' key is pressed
        if (GetAsyncKeyState('K') & 1) {
            // Your custom kill logic goes here!
            // Example: writing a value to the player's health memory address
            MessageBoxA(
                NULL,                                     // Owner window (NULL means no owner)
                "The DLL was injected and hotkey works!", // The message inside the popup
                "Hamsterball Mod Test",                   // The title of the popup window
                MB_OK | MB_ICONINFORMATION                // Buttons and icon type
            );
        }

        // Pause the thread for 10 milliseconds to prevent CPU hogging
        Sleep(10);
    }
    return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Disable thread library calls for optimization
        DisableThreadLibraryCalls(hModule);

        // Spawn our custom mod thread alongside the game
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ModThread, hModule, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
