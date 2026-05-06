#pragma once
#include <windows.h>

// --- TYPEDEFS ---
typedef void(__fastcall* FindRespawnPointFunc)(void* ecx_playerObject, void* edx_dummy);
typedef void(__fastcall* PlayerUpdateFunc)(void* ecx_player, void* edx_dummy);
typedef void* (__fastcall* OptionsMenuFunc)(void* this_ptr, void* edx_dummy, int param_1, int param_2);
typedef void(__fastcall* AddMenuButtonFunc)(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, const char* style, const char* unk);
typedef void(__fastcall* AddSpacerFunc)(void* this_ptr, void* edx_dummy, int height);
typedef void(__fastcall* OptionsClickFunc)(void* this_ptr, void* edx_dummy, const char* clicked_id);
typedef void(__fastcall* UpdateButtonTextFunc)(void* this_ptr, void* edx_dummy, const char* newText, const char* id);
typedef const char* (__fastcall* GetLevelNameFunc)(void* ecx_obj, void* edx_dummy);

// --- EXTERN GLOBALS ---
extern bool g_ModEnabled;
extern void* g_StolenPlayer;

// Function Pointers
extern FindRespawnPointFunc Original_FindRespawn;
extern PlayerUpdateFunc Original_PlayerUpdate;
extern OptionsMenuFunc Original_OptionsMenu;
extern AddMenuButtonFunc Original_AddMenuButton;
extern AddSpacerFunc Original_AddSpacer;
extern OptionsClickFunc Original_OptionsClick;
extern UpdateButtonTextFunc Game_UpdateButtonText;
extern GetLevelNameFunc Original_GetLevelName;