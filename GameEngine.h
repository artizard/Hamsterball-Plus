#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <math.h>

// --- TYPEDEFS ---
typedef void(__fastcall* FindRespawnPointFunc)(void* ecx_playerObject, void* edx_dummy);
typedef void(__fastcall* PlayerUpdateFunc)(void* ecx_player, void* edx_dummy);
typedef void* (__fastcall* OptionsMenuFunc)(void* this_ptr, void* edx_dummy, int param_1, int param_2);
typedef void(__fastcall* AddMenuButtonFunc)(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, const char* style, const char* unk);
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
typedef void(__fastcall* BaseCollideCheckFunc)(void* this_ptr, void* edx, int* ballPhysics, int* collisionData);
typedef void(__thiscall* GeometryBinderFunc)(void* meshWorld_this, const char* searchString, void* destStruct);
typedef void(__fastcall* MasterLevelSetupFunc)(int* param_1);

// --- EXTERN GLOBALS ---
extern bool g_CheatSpeed;
extern bool g_CheatJump;
extern bool g_CheatNoBreak;
extern bool g_CheatTopDown;

extern void* g_StolenPlayer;

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
extern BaseCollideCheckFunc Original_BaseCollideCheck;
extern GeometryBinderFunc Original_BindGeometry;
extern MasterLevelSetupFunc Original_MasterLevelSetup;

// --- Vector3 ---

// Simple 3D Vector struct
struct Vec3 {
    float x, y, z;
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

// Math Helpers
inline Vec3 Subtract(Vec3 a, Vec3 b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vec3 Cross(Vec3 a, Vec3 b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 Normalize(Vec3 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f) return Vec3(0, 0, 0);
    return Vec3(v.x / length, v.y / length, v.z / length);
}

// Builds a DirectX 8 compatible View Matrix (16 floats)
inline void BuildCustomViewMatrix(float* outMatrix, Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 zaxis = Normalize(Subtract(target, eye));
    Vec3 xaxis = Normalize(Cross(up, zaxis));
    Vec3 yaxis = Cross(zaxis, xaxis);

    outMatrix[0] = xaxis.x;           outMatrix[1] = yaxis.x;           outMatrix[2] = zaxis.x;           outMatrix[3] = 0.0f;
    outMatrix[4] = xaxis.y;           outMatrix[5] = yaxis.y;           outMatrix[6] = zaxis.y;           outMatrix[7] = 0.0f;
    outMatrix[8] = xaxis.z;           outMatrix[9] = yaxis.z;           outMatrix[10] = zaxis.z;          outMatrix[11] = 0.0f;
    outMatrix[12] = -Dot(xaxis, eye); outMatrix[13] = -Dot(yaxis, eye); outMatrix[14] = -Dot(zaxis, eye); outMatrix[15] = 1.0f;
}