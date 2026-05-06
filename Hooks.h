#pragma once
#include <windows.h>

void __fastcall Hooked_PlayerUpdate(void* ecx_player, void* edx_dummy);
void* __fastcall Hooked_OptionsMenu(void* this_ptr, void* edx_dummy, int param_1, int param_2);
void __fastcall Hooked_OptionsClick(void* this_ptr, void* edx_dummy, const char* clicked_id);
void __fastcall Hooked_AddMenuButton(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, const char* style, const char* unk);
