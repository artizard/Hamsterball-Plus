#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <math.h>
#include "HamsterballAPI.h"
#include <map>

// --- TYPEDEFS ---
typedef void(__fastcall* FindRespawnPointFunc)(void* ecx_playerObject, void* edx_dummy);
typedef void(__fastcall* PlayerUpdateFunc)(Ball* ecx_player, void* edx_dummy);
typedef void* (__fastcall* OptionsMenuFunc)(void* this_ptr, void* edx_dummy, int param_1, int param_2);
typedef void(__fastcall* AddMenuButtonFunc)(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, float a, const void* img);
typedef void(__fastcall* AddSpacerFunc)(void* this_ptr, void* edx_dummy, int height);
typedef void(__fastcall* OptionsClickFunc)(void* this_ptr, void* edx_dummy, const char* clicked_id);
typedef void(__fastcall* UpdateButtonTextFunc)(void* this_ptr, void* edx_dummy, const char* newText, const char* id);
typedef const char* (__fastcall* GetLevelNameFunc)(void* ecx_obj, void* edx_dummy);
typedef void(__fastcall* DrawHUDTextFunc)(
    void* this_ptr, void* edx_dummy,
    const char* text,
    int x, int y,
    int shadowOffsetX, int shadowOffsetY,
    DWORD c1_vtable, float c1_r, float c1_g, float c1_b, float c1_a,
    DWORD c2_vtable, float c2_r, float c2_g, float c2_b, float c2_a
    );
typedef void* (__fastcall* CreateColorFunc)(void* colorStruct, void* edx_dummy, float r, float g, float b, float a);
typedef void(__thiscall* HudManagerFunc)(void* this_ptr, void* param_1);
typedef void(__fastcall* RenderApplyFunc)(void* this_ptr, void* edx_dummy, float* viewMatrix);
//typedef void(__fastcall* BaseCollideCheckFunc)(void* this_ptr, void* edx, Ball* colliding_ball, int* collisionData);
typedef void(__thiscall* GeometryBinderFunc)(void* meshWorld_this, const char* searchString, void* destStruct);
typedef void(__fastcall* MasterLevelSetupFunc)(int* param_1);
typedef void(__thiscall* RenderDynamic_t)(void* this_ptr, void* param_1);
typedef void(__fastcall* PollInputsFunc)(void* self);
typedef void(__fastcall* SaveConfigFunc)(App* app); 
typedef void(__fastcall* GameUpdateFunc)(App* app);
typedef void(__thiscall* ApplyForceFunc)(void* player, float x, float y, float z, float magnitude);
typedef void(__thiscall* CollisionCheckFunc)(void* this_ptr, Ball* colliding_ball, int* param_3);


// --- EXTERN GLOBALS ---
extern Ball* g_Player;
extern Ball* g_Player2;
extern Ball* g_Player3;
extern Ball* g_Player4;
inline std::vector<Ball*> g_Enemies; // 8 ball or ball on sky race 
extern App* g_App; 
inline std::vector<HamsterballAPI*> g_Mods; 
inline std::map<std::string, int> g_CustomControls; // <controlID, dikCode> 

struct ThemeConfig {
    float MenuBodyR, MenuBodyG, MenuBodyB, MenuBodyA;
    float MenuHeaderR, MenuHeaderG, MenuHeaderB, MenuHeaderA;
};
struct LevelConfig {
    float ColorR, ColorG, ColorB;
    float BlotR, BlotB, BlotG;
    std::string RaceName;
    std::string ArenaName;
};

extern ThemeConfig g_Theme;
extern std::vector<LevelConfig> g_LevelConfigs;
extern bool g_ShowCheats;
extern bool g_ShowConsole;


// Function Pointers
extern FindRespawnPointFunc Original_FindRespawn;
extern PlayerUpdateFunc Original_PlayerUpdate;
extern OptionsMenuFunc Original_OptionsMenu;
extern AddMenuButtonFunc Original_AddMenuButton;
extern AddSpacerFunc Original_AddSpacer;
extern OptionsClickFunc Original_OptionsClick;
extern UpdateButtonTextFunc Game_UpdateButtonText;
extern GetLevelNameFunc Original_GetLevelName;
extern DrawHUDTextFunc Original_DrawHUDText;
extern CreateColorFunc Original_CreateColor;
extern HudManagerFunc Original_HudManager;
extern RenderApplyFunc Original_RenderApply;
//extern BaseCollideCheckFunc Original_BaseCollideCheck;
extern GeometryBinderFunc Original_BindGeometry;
extern MasterLevelSetupFunc Original_MasterLevelSetup;
extern RenderDynamic_t Original_RenderDynamic;
extern SaveConfigFunc SaveConfig; 

extern PollInputsFunc Original_PollInputs;
extern GameUpdateFunc Original_GameUpdate; 
extern ApplyForceFunc ApplyForce; 
extern CollisionCheckFunc Original_CollisionCheck; 

void UpdateBallReferences(); 