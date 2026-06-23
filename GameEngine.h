#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <math.h>
#include "HamsterballAPI.h"
#include <map>
#include <functional>

struct Resolution {
    int width;
    int height;

    bool operator==(const Resolution& other) const {
        return width == other.width && height == other.height;
    }
    bool operator<(const Resolution& other) const {
        if (width == other.width) return height < other.height;
        return width < other.width; 
    }
};

// --- TYPEDEFS ---
typedef void(__fastcall* FindRespawnPointFunc)(void* ecx_playerObject, void* edx_dummy);
typedef void(__fastcall* BallUpdateFunc)(Ball* ball, void* edx_dummy);
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
typedef void(__thiscall* SliderOptionHandlerFunc)(void* this_ptr, char* sliderID, int inputDirection);
typedef Vec3*(__thiscall* LevelRaycastFunc)(Collision* collision, Vec3* out, Vec3 origin, Vec3 direction, float max_dist);
typedef void(__fastcall* PlaySoundFunc)(void* soundEffect, float volume);
typedef void(__thiscall* Play3dSoundFunc)(void* soundEffect, Vec3 position, float volume);
typedef void(__thiscall* ShowBallMessageFunc)(Ball* ball, char* message); 
typedef void(__fastcall* RenderTextLoopFunc)(void* this_ptr); 
typedef void(__thiscall* DrawTextNoShadowFunc)(void* font, char* text, int x, int y, void* vtable, float r, float g, float b, float a);
typedef void(__thiscall* DrawGameTextFunc)(void* font, char* text, int x, int y, int shadow_x, int shadow_y, void* vtable1, float t_r, float t_g, float t_b, float t_a, void* vtable2, float s_r, float s_g, float s_b, float s_a);

// --- EXTERN GLOBALS ---
extern Ball* g_Player;
extern Ball* g_Player2;
extern Ball* g_Player3;
extern Ball* g_Player4;
inline std::vector<Ball*> g_Enemies; // 8 ball or ball on sky race 
extern App* g_App; 
extern PhysicsConstants* g_PhysicsConstants; 
inline std::vector<HamsterballAPI*> g_Mods; 
extern int* g_Timer; 
inline std::vector<Resolution> g_AvailableResolutions; 

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
struct TimedMessage {
    ULONGLONG endTime; 
    std::string text; 
    CustomText params;
};

// using std::less<> here to be able to look up controls from the map without converting char* to string, which requires memory allocation (optimization thing)
inline std::map<std::string, CustomControl, std::less<>> g_CustomControls; // <controlID, CustomControl struct> 
extern ThemeConfig g_Theme;
extern std::vector<LevelConfig> g_LevelConfigs;
extern bool g_ShowCheats;
extern bool g_ShowConsole;

// So theres a function that modders can use that displays a message for x amount of time. I need to store this between renders so I'm using this data structure.
// I'm using swap and pop method for caching efficiency to hopefully avoid performance issues. 
inline std::vector<TimedMessage> g_TimedMessages; 

// Function Pointers
extern FindRespawnPointFunc Original_FindRespawn;
extern BallUpdateFunc Original_BallUpdate;
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
extern SaveConfigFunc Original_SaveConfig; 

extern PollInputsFunc Original_PollInputs;
extern GameUpdateFunc Original_GameUpdate; 
extern ApplyForceFunc ApplyForce; 
extern CollisionCheckFunc Original_CollisionCheck; 
extern SliderOptionHandlerFunc Original_SliderOptionHandler;
extern LevelRaycastFunc LevelRaycast; 

extern PlaySoundFunc PlaySoundEffect;
extern Play3dSoundFunc Play3dSoundEffect;
extern ShowBallMessageFunc ShowBallMessage;
extern RenderTextLoopFunc Original_RenderTextLoop; 
extern DrawTextNoShadowFunc DrawTextNoShadow;
extern DrawGameTextFunc DrawGameText; 

void UpdateBallReferences(); 