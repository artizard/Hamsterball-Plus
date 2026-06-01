#pragma once
#include <windows.h>

void __fastcall Hooked_PlayerUpdate(void* ecx_player, void* edx_dummy);
void* __fastcall Hooked_OptionsMenu(void* this_ptr, void* edx_dummy, int param_1, int param_2);
void __fastcall Hooked_OptionsClick(void* this_ptr, void* edx_dummy, const char* clicked_id);
void __fastcall Hooked_AddMenuButton(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, float a, const void* img);
const char* __fastcall Hooked_GetLevelName(void* ecx_obj, void* edx_dummy);
void __fastcall Hooked_DrawHUDText(void* this_ptr, void* edx_dummy, const char* text, int x, int y, int shadowOffsetX, int shadowOffsetY, DWORD c1_vtable, float c1_r, float c1_g, float c1_b, float c1_a, DWORD c2_vtable, float c2_r, float c2_g, float c2_b, float c2_a);
void* __fastcall Hooked_CreateColor(void* colorStruct, void* edx_dummy, float r, float g, float b, float a);
void __fastcall Hooked_HudManager(void* this_ptr, void* edx_dummy, void* param_1);
void __fastcall Hooked_RenderApply(void* this_ptr, void* edx_dummy, float* viewMatrix);
void __fastcall Hooked_RenderDynamic(void* this_ptr, void* edx_dummy, void* param_1);
void __fastcall Hooked_Shatter1(void* this_ptr, void* edx_dummy, int param_1);
void __fastcall Hooked_Shatter2(void* param_1, void* edx_dummy);
void __fastcall Hooked_Shatter3(void* param_1, void* edx_dummy);