#include "pch.h"
#include "Hooks.h"
#include "GameEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

ThemeConfig g_Theme;
std::vector<LevelConfig> g_LevelConfigs = {};
bool g_ShowCheats = true;
bool g_ShowConsole = false;

// --- HELPER FUNCTIONS ---

// Helper function to overwrite game code (Byte Patching)
void PatchMemory(DWORD address, const char* bytes, size_t size) {
    DWORD oldProtect;

    // Unlock the memory so we can write to it
    VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    // Copy our new bytes over the old ones
    memcpy((void*)address, bytes, size);

    // Lock it back up
    VirtualProtect((void*)address, size, oldProtect, &oldProtect);
}

const char* GetModIniPath() {
    static std::string iniPath = "";

    if (iniPath.empty()) {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        std::string pathStr = exePath;
        size_t lastSlash = pathStr.find_last_of('\\');
        iniPath = pathStr.substr(0, lastSlash) + "\\ModConfig.ini";
    }

    return iniPath.c_str();
}

// Helper function to read a float from the INI file
float ReadIniFloat(const char* section, const char* key, float defaultValue, const char* filePath) {
    char buffer[64];
    GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), filePath);
    if (buffer[0] == '\0') return defaultValue;
    return (float)atof(buffer);
}

// Helper function to read ini file
void ReloadINI() {
    const char* path = GetModIniPath();

    // Read Values
    g_Theme.MenuBodyR = ReadIniFloat("Theme", "MenuBodyR", 0.0f, path);
    g_Theme.MenuBodyG = ReadIniFloat("Theme", "MenuBodyG", 0.0f, path);
    g_Theme.MenuBodyB = ReadIniFloat("Theme", "MenuBodyB", 1.0f, path);
    g_Theme.MenuBodyA = ReadIniFloat("Theme", "MenuBodyA", 0.75f, path);

    g_Theme.MenuHeaderR = ReadIniFloat("Theme", "MenuHeaderR", 0.0f, path);
    g_Theme.MenuHeaderG = ReadIniFloat("Theme", "MenuHeaderG", 1.0f, path);
    g_Theme.MenuHeaderB = ReadIniFloat("Theme", "MenuHeaderB", 1.0f, path);
    g_Theme.MenuHeaderA = ReadIniFloat("Theme", "MenuHeaderA", 0.75f, path);

    g_ShowCheats = GetPrivateProfileIntA("Config", "ShowCheats", 1, path) != 0;
    g_ShowConsole = GetPrivateProfileIntA("Config", "ShowConsole", 0, path) != 0;

    g_LevelConfigs.clear();
    for (int i = 0; i < 15; i++) {
        LevelConfig config;
        std::string s = std::to_string(i);
        const char* iStr = s.c_str();

        config.ColorR = ReadIniFloat(iStr, "ColorR", -1.f, path);
        config.ColorG = ReadIniFloat(iStr, "ColorG", -1.f, path);
        config.ColorB = ReadIniFloat(iStr, "ColorB", -1.f, path);

        // Use temporary local buffers, let std::string safely copy the data
        char tempRace[256], tempArena[256];
        GetPrivateProfileStringA(iStr, "RaceName", "", tempRace, sizeof(tempRace), path);
        GetPrivateProfileStringA(iStr, "ArenaName", "", tempArena, sizeof(tempArena), path);

        // This makes a permanent, safe copy of the string in memory
        config.RaceName = tempRace;
        config.ArenaName = tempArena;

        g_LevelConfigs.push_back(config);
    }
}

// Helper to convert "LOCKED" strings back to numeric IDs
const int NormalizeLevelID(const char* originalId, bool& outIsLocked) {
    outIsLocked = false;

    // Use _stricmp to ignore case, since the devs inconsistently capitalized "Glass"
    if (_stricmp(originalId, "LOCKED DIZZY") == 0) { outIsLocked = true; return 3; }
    if (_stricmp(originalId, "LOCKED TOWER") == 0) { outIsLocked = true; return 4; }
    if (_stricmp(originalId, "LOCKED UP") == 0) { outIsLocked = true; return 5; }
    if (_stricmp(originalId, "LOCKED NEON") == 0) { outIsLocked = true; return 6; }
    if (_stricmp(originalId, "LOCKED EXPERT") == 0) { outIsLocked = true; return 7; }
    if (_stricmp(originalId, "LOCKED ODD") == 0) { outIsLocked = true; return 8; }
    if (_stricmp(originalId, "LOCKED TOOB") == 0) { outIsLocked = true; return 9; }
    if (_stricmp(originalId, "LOCKED WOBBLY") == 0) { outIsLocked = true; return 10; }
    if (_stricmp(originalId, "LOCKED GLASS") == 0) { outIsLocked = true; return 11; }
    if (_stricmp(originalId, "LOCKED SKY") == 0) { outIsLocked = true; return 12; }
    if (_stricmp(originalId, "LOCKED MASTER") == 0) { outIsLocked = true; return 13; }
    if (_stricmp(originalId, "LOCKED IMPOSSIBLE") == 0) { outIsLocked = true; return 14; }

    try {
        return std::stoi(originalId);
    }
    catch (...) {
        // If it's a string like "OPTIONS" or "CHEAT_SPEED", ignore it
        return -1;
    }
}

// Helper to convert HUD strings back to INI IDs AND determine the game mode
const int GetLevelIdFromHUDText(const char* text, bool& isArena) {
    isArena = false; // Default to Race

    // WARM-UP
    if (_stricmp(text, "WARM-UP RACE") == 0) return 0;
    if (_stricmp(text, "WARM-UP ARENA") == 0) { isArena = true; return 0; }

    // BEGINNER
    if (_stricmp(text, "BEGINNER RACE") == 0) return 1;
    if (_stricmp(text, "BEGINNER ARENA") == 0) { isArena = true; return 1; }

    // INTERMEDIATE
    if (_stricmp(text, "INTERMEDIATE RACE") == 0) return 2;
    if (_stricmp(text, "INTERMEDIATE ARENA") == 0) { isArena = true; return 2; }

    // DIZZY
    if (_stricmp(text, "DIZZY RACE") == 0) return 3;
    if (_stricmp(text, "DIZZY ARENA") == 0) { isArena = true; return 3; }

    // TOWER
    if (_stricmp(text, "TOWER RACE") == 0) return 4;
    if (_stricmp(text, "TOWER ARENA") == 0) { isArena = true; return 4; }

    // UP
    if (_stricmp(text, "UP RACE") == 0) return 5;
    if (_stricmp(text, "UP ARENA") == 0) { isArena = true; return 5; }

    // NEON
    if (_stricmp(text, "NEON RACE") == 0) return 6;
    if (_stricmp(text, "NEON ARENA") == 0) { isArena = true; return 6; }

    // EXPERT
    if (_stricmp(text, "EXPERT RACE") == 0) return 7;
    if (_stricmp(text, "EXPERT ARENA") == 0) { isArena = true; return 7; }

    // ODD
    if (_stricmp(text, "ODD RACE") == 0) return 8;
    if (_stricmp(text, "ODD ARENA") == 0) { isArena = true; return 8; }

    // TOOB
    if (_stricmp(text, "TOOB RACE") == 0) return 9;
    if (_stricmp(text, "TOOB ARENA") == 0) { isArena = true; return 9; }

    // WOBBLY
    if (_stricmp(text, "WOBBLY RACE") == 0) return 10;
    if (_stricmp(text, "WOBBLY ARENA") == 0) { isArena = true; return 10; }

    // GLASS
    if (_stricmp(text, "GLASS RACE") == 0) return 11;
    if (_stricmp(text, "GLASS ARENA") == 0) { isArena = true; return 11; }

    // SKY
    if (_stricmp(text, "SKY RACE") == 0) return 12;
    if (_stricmp(text, "SKY ARENA") == 0) { isArena = true; return 12; }

    // MASTER
    if (_stricmp(text, "MASTER RACE") == 0) return 13;
    if (_stricmp(text, "MASTER ARENA") == 0) { isArena = true; return 13; }

    // IMPOSSIBLE
    if (_stricmp(text, "IMPOSSIBLE RACE") == 0) return 14;
    if (_stricmp(text, "IMPOSSIBLE ARENA") == 0) { isArena = true; return 14; }

    return -1;
}

// --- BYTE PATCH FUNCTIONS ---

// Toggles the No Fall Damage byte patches (originally by XRow)
void ApplyNoFallDamage(bool enable) {

    // Get the base address of Hamsterball.exe
    DWORD baseAddr = (DWORD)GetModuleHandle(NULL);

    if (enable) {
        // [ENABLE] - Overwrite with NOPs (0x90)
        PatchMemory(baseAddr + 0xC767, "\x90\x90\x90\x90\x90\x90\x90", 7);
        PatchMemory(baseAddr + 0xF22D, "\x90\x90\x90\x90\x90\x90\x90", 7);
        PatchMemory(baseAddr + 0x75C9, "\x90\x90\x90\x90\x90\x90", 6);
        PatchMemory(baseAddr + 0xC761, "\x90\x90\x90\x90\x90\x90", 6);
        PatchMemory(baseAddr + 0xF226, "\x90\x90\x90\x90\x90\x90\x90", 7);
    }
    else {
        // [DISABLE] - Restore original game code
        PatchMemory(baseAddr + 0xC767, "\xC6\x85\xE9\x02\x00\x00\x01", 7);
        PatchMemory(baseAddr + 0xF22D, "\xC6\x86\xE9\x02\x00\x00\x01", 7);
        PatchMemory(baseAddr + 0x75C9, "\xFF\x86\xEC\x02\x00\x00", 6);
        PatchMemory(baseAddr + 0xC761, "\x88\x85\x68\x07\x00\x00", 6);
        PatchMemory(baseAddr + 0xF226, "\xC6\x86\x68\x07\x00\x00\x00", 7);
    }
}

// --- HOOK IMPLEMENTATIONS ---

// Hooked Player Update Loop (for getting player object pointer)
void __fastcall Hooked_PlayerUpdate(void* ecx_player, void* edx_dummy) {

    // Look at offset 0x18 to check the "Controller ID"
    int* controllerID = (int*)((DWORD)ecx_player + 0x18);

    // ONLY steal the pointer if it is Player 1 (ID = 0)
    // This will safely ignore the 8-Ball (which is likely -1)
    if (*controllerID == 0) {
        g_StolenPlayer = ecx_player;

        //float* gravity = (float*)((DWORD)ecx_player + 0x2a4);
        //// normal is around 5
        //*gravity = 10.f; 

        // --- THE SPEED LIMIT BREAKER ---
        if (g_CheatSpeed) {
            // Break moving speed limit
            float* masterSpeed = (float*)((DWORD)ecx_player + 0x188);
            *masterSpeed = 500.0f;

            DWORD* physicsObjPtr = (DWORD*)((DWORD)ecx_player + 0x1a4);
            if (physicsObjPtr != nullptr && *physicsObjPtr != 0) {
                DWORD physicsObj = *physicsObjPtr;

                // Break the global physics speed cap
                float* maxSpeed = (float*)(physicsObj + 0xc70);
                *maxSpeed = 9999.0f;

                // Turn off air resistance
                float* drag = (float*)(physicsObj + 0xc68);
                *drag = 0.0f;
            }
        }
    }

    Original_PlayerUpdate(ecx_player, edx_dummy);
}

// Adding custom option
void* __fastcall Hooked_OptionsMenu(void* this_ptr, void* edx_dummy, int param_1, int param_2) {

    // Call the original function, save the pointer that it returns
    void* menuPointer = Original_OptionsMenu(this_ptr, edx_dummy, param_1, param_2);

    if (g_ShowCheats) {
        DWORD baseAddr = (DWORD)GetModuleHandle(NULL);
        
        // Add spacer
        Original_AddSpacer(this_ptr, nullptr, 10);

        // Color Object vtable pointer
        DWORD vtableAddr = baseAddr + 0xCF300;

        // Add custom options
        const char* speedText = g_CheatSpeed ? "UNCAP SPEED: YES" : "UNCAP SPEED: NO";
        Original_AddMenuButton(this_ptr, nullptr, speedText, "CHEAT_SPEED", vtableAddr, 1.0f, 1.0f, 1.0f, "j", nullptr);

        const char* jumpText = g_CheatJump ? "JUMPING: YES" : "JUMPING: NO";
        Original_AddMenuButton(this_ptr, nullptr, jumpText, "CHEAT_JUMP", vtableAddr, 1.0f, 1.0f, 1.0f, "j", nullptr);

        const char* dmgText = g_CheatNoBreak ? "NO BREAK: YES" : "NO BREAK: NO";
        Original_AddMenuButton(this_ptr, nullptr, dmgText, "CHEAT_NOBREAK", vtableAddr, 1.0f, 1.0f, 1.0f, "j", nullptr);
    }

    // Return the saved pointer
    return menuPointer;
}

// Logic for clicking options menu buttons
void __fastcall Hooked_OptionsClick(void* this_ptr, void* edx_dummy, const char* clicked_id) {

    // Speed Cheat
    if (strcmp(clicked_id, "CHEAT_SPEED") == 0) {
        g_CheatSpeed = !g_CheatSpeed;
        const char* newText = g_CheatSpeed ? "UNCAP SPEED: YES" : "UNCAP SPEED: NO";
        // Redraw button
        Game_UpdateButtonText(this_ptr, nullptr, newText, "CHEAT_SPEED");
        return;
    }

    // Jump Cheat
    if (strcmp(clicked_id, "CHEAT_JUMP") == 0) {
        g_CheatJump = !g_CheatJump;
        const char* newText = g_CheatJump ? "JUMPING: YES" : "JUMPING: NO";
        // Redraw button
        Game_UpdateButtonText(this_ptr, nullptr, newText, "CHEAT_JUMP");
        return;
    }

    // No Break Cheat
    if (strcmp(clicked_id, "CHEAT_NOBREAK") == 0) {
        g_CheatNoBreak = !g_CheatNoBreak;

        // Apply patch
        ApplyNoFallDamage(g_CheatNoBreak);

        const char* newText = g_CheatNoBreak ? "NO BREAK: YES" : "NO BREAK: NO";
        Game_UpdateButtonText(this_ptr, nullptr, newText, "CHEAT_NOBREAK");
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

// Allow custom level names and colors
void __fastcall Hooked_AddMenuButton(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, const char* style, const char* unk) {
    char* customName = nullptr;
    const char* finalDisplayText = displayText;

    if (displayText != nullptr && id != nullptr) {

        // Normalize the ID (Converts "LOCKED IMPOSSIBLE" to "14")
        bool isLocked = false;
        const int lookupId = NormalizeLevelID(id, isLocked);

        if (lookupId >= 0 && lookupId < (int)g_LevelConfigs.size()) {

            const char* customName = nullptr;

            if (strstr(displayText, "ARENA") != nullptr) {
                customName = g_LevelConfigs[lookupId].ArenaName.c_str();
            }
            else if (strstr(displayText, "RACE") != nullptr) {
                customName = g_LevelConfigs[lookupId].RaceName.c_str();
            }

            if (customName != nullptr && customName[0] != '\0') {
                finalDisplayText = customName;
            }

            // Don't change colors if level is locked
            if (!isLocked) {
                if (g_LevelConfigs[lookupId].ColorR != -1.f) r = g_LevelConfigs[lookupId].ColorR;
                if (g_LevelConfigs[lookupId].ColorG != -1.f) g = g_LevelConfigs[lookupId].ColorG;
                if (g_LevelConfigs[lookupId].ColorB != -1.f) b = g_LevelConfigs[lookupId].ColorB;
            }
        }
    }

    // Pass everything to the engine
    Original_AddMenuButton(this_ptr, edx_dummy, finalDisplayText, id, vtable, r, g, b, style, unk);
}

const char* __fastcall Hooked_GetLevelName(void* ecx_obj, void* edx_dummy) {

    // The engine stores the Level ID at offset + 8
    int levelID = *(int*)((DWORD)ecx_obj + 8);

    // Safety check: ensure the ID is valid
    if (levelID >= 0 && levelID < g_LevelConfigs.size()) {
        const char* customName = nullptr;

        // Try to read the custom name
        customName = g_LevelConfigs[levelID].RaceName.c_str();

        // If we found one, return our custom permanent pointer
        if (customName[0] != '\0') {
            return customName; // Return our custom string
        }
    }

    // Otherwise, return the original game's string
    return Original_GetLevelName(ecx_obj, edx_dummy);
}

// Intercept when the game tries to draw the level name when you start a level
void __fastcall Hooked_DrawHUDText(void* this_ptr, void* edx_dummy, const char* text, int x, int y, int shadowOffsetX, int shadowOffsetY, DWORD c1_vtable, float c1_r, float c1_g, float c1_b, float c1_a, DWORD c2_vtable, float c2_r, float c2_g, float c2_b, float c2_a) {

    const char* finalDisplayText = text;

    if (text != nullptr) {

        // Let our helper figure out the ID and the Context
        bool isArena = false;
        const int levelId = GetLevelIdFromHUDText(text, isArena);

        if (levelId >= 0 && levelId < g_LevelConfigs.size()) {

            // Dynamically select the correct string from the struct
            const std::string& customStr = isArena ? g_LevelConfigs[levelId].ArenaName : g_LevelConfigs[levelId].RaceName;

            // If the string isn't empty, safely grab its pointer
            if (!customStr.empty()) {
                finalDisplayText = customStr.c_str();
            }
        }
    }

    // Pass everything back to the engine
    Original_DrawHUDText(this_ptr, edx_dummy, finalDisplayText, x, y, shadowOffsetX, shadowOffsetY, c1_vtable, c1_r, c1_g, c1_b, c1_a, c2_vtable, c2_r, c2_g, c2_b, c2_a);
}

void* __fastcall Hooked_CreateColor(void* colorStruct, void* edx_dummy, float r, float g, float b, float a) {

    //if (r == 0.0f && g == 0.0f && b == 1.0f) {
    //    printf("Found UI Color -> R: %f, G: %f, B: %f, A: %f\n", r, g, b, a);
    //}

    // Intercept the "Menu Body" Color
    if (r == 0.0f && g == 0.0f && b == 1.0f && a == 0.75f) {
        
        r = g_Theme.MenuBodyR;
        g = g_Theme.MenuBodyG;
        b = g_Theme.MenuBodyB;
        a = g_Theme.MenuBodyA;
    }

    // Intercept the "Menu Header" Color
    else if (r == 0.5f && g == 0.5f && b == 1.0f && a == 0.75f) {

        r = g_Theme.MenuHeaderR;
        g = g_Theme.MenuHeaderG;
        b = g_Theme.MenuHeaderB;
        a = g_Theme.MenuHeaderA;
    }

    // Pass the filtered color to the actual engine
    return Original_CreateColor(colorStruct, edx_dummy, r, g, b, a);
}