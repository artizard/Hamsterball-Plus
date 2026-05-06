#include "pch.h"
#include "Hooks.h"
#include "GameEngine.h"
#include <stdio.h>
#include <stdlib.h>

// --- HELPER FUNCTIONS ---

// Helper function to read a float from the INI file
float ReadIniFloat(const char* section, const char* key, float defaultValue, const char* filePath) {
    char buffer[64];
    GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), filePath);
    if (buffer[0] == '\0') return defaultValue;
    return (float)atof(buffer);
}

// Helper to convert "LOCKED" strings back to numeric IDs
const char* NormalizeLevelID(const char* originalId, bool& outIsLocked) {
    outIsLocked = false;

    // Use _stricmp to ignore case, since the devs inconsistently capitalized "Glass"
    if (_stricmp(originalId, "LOCKED DIZZY") == 0) { outIsLocked = true; return "3"; }
    if (_stricmp(originalId, "LOCKED TOWER") == 0) { outIsLocked = true; return "4"; }
    if (_stricmp(originalId, "LOCKED UP") == 0) { outIsLocked = true; return "5"; }
    if (_stricmp(originalId, "LOCKED NEON") == 0) { outIsLocked = true; return "6"; }
    if (_stricmp(originalId, "LOCKED EXPERT") == 0) { outIsLocked = true; return "7"; }
    if (_stricmp(originalId, "LOCKED ODD") == 0) { outIsLocked = true; return "8"; }
    if (_stricmp(originalId, "LOCKED TOOB") == 0) { outIsLocked = true; return "9"; }
    if (_stricmp(originalId, "LOCKED WOBBLY") == 0) { outIsLocked = true; return "10"; }
    if (_stricmp(originalId, "LOCKED GLASS") == 0) { outIsLocked = true; return "11"; }
    if (_stricmp(originalId, "LOCKED SKY") == 0) { outIsLocked = true; return "12"; }
    if (_stricmp(originalId, "LOCKED MASTER") == 0) { outIsLocked = true; return "13"; }
    if (_stricmp(originalId, "LOCKED IMPOSSIBLE") == 0) { outIsLocked = true; return "14"; }

    return originalId; // If it's already a number like "14", just return it as-is
}

// Helper to convert HUD strings back to INI IDs AND determine the game mode
const char* GetLevelIdFromHUDText(const char* text, bool& isArena) {
    isArena = false; // Default to Race

    // WARM-UP
    if (_stricmp(text, "WARM-UP RACE") == 0) return "0";
    if (_stricmp(text, "WARM-UP ARENA") == 0) { isArena = true; return "0"; }

    // BEGINNER
    if (_stricmp(text, "BEGINNER RACE") == 0) return "1";
    if (_stricmp(text, "BEGINNER ARENA") == 0) { isArena = true; return "1"; }

    // INTERMEDIATE
    if (_stricmp(text, "INTERMEDIATE RACE") == 0) return "2";
    if (_stricmp(text, "INTERMEDIATE ARENA") == 0) { isArena = true; return "2"; }

    // DIZZY
    if (_stricmp(text, "DIZZY RACE") == 0) return "3";
    if (_stricmp(text, "DIZZY ARENA") == 0) { isArena = true; return "3"; }

    // TOWER
    if (_stricmp(text, "TOWER RACE") == 0) return "4";
    if (_stricmp(text, "TOWER ARENA") == 0) { isArena = true; return "4"; }

    // UP
    if (_stricmp(text, "UP RACE") == 0) return "5";
    if (_stricmp(text, "UP ARENA") == 0) { isArena = true; return "5"; }

    // NEON
    if (_stricmp(text, "NEON RACE") == 0) return "6";
    if (_stricmp(text, "NEON ARENA") == 0) { isArena = true; return "6"; }

    // EXPERT
    if (_stricmp(text, "EXPERT RACE") == 0) return "7";
    if (_stricmp(text, "EXPERT ARENA") == 0) { isArena = true; return "7"; }

    // ODD
    if (_stricmp(text, "ODD RACE") == 0) return "8";
    if (_stricmp(text, "ODD ARENA") == 0) { isArena = true; return "8"; }

    // TOOB
    if (_stricmp(text, "TOOB RACE") == 0) return "9";
    if (_stricmp(text, "TOOB ARENA") == 0) { isArena = true; return "9"; }

    // WOBBLY
    if (_stricmp(text, "WOBBLY RACE") == 0) return "10";
    if (_stricmp(text, "WOBBLY ARENA") == 0) { isArena = true; return "10"; }

    // GLASS
    if (_stricmp(text, "GLASS RACE") == 0) return "11";
    if (_stricmp(text, "GLASS ARENA") == 0) { isArena = true; return "11"; }

    // SKY
    if (_stricmp(text, "SKY RACE") == 0) return "12";
    if (_stricmp(text, "SKY ARENA") == 0) { isArena = true; return "12"; }

    // MASTER
    if (_stricmp(text, "MASTER RACE") == 0) return "13";
    if (_stricmp(text, "MASTER ARENA") == 0) { isArena = true; return "13"; }

    // IMPOSSIBLE
    if (_stricmp(text, "IMPOSSIBLE RACE") == 0) return "14";
    if (_stricmp(text, "IMPOSSIBLE ARENA") == 0) { isArena = true; return "14"; }

    return nullptr;
}

// --- HOOK IMPLEMENTATIONS ---

// Hooked Player Update Loop (for getting player object pointer)
void __fastcall Hooked_PlayerUpdate(void* ecx_player, void* edx_dummy) {
    // Passively steal the player pointer 60 times a second
    g_StolenPlayer = ecx_player;

    // Resume normal game logic
    Original_PlayerUpdate(ecx_player, edx_dummy);
}

// Adding custom option
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

// Logic for clicking options menu buttons
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

// Allow custom level names and colors
void __fastcall Hooked_AddMenuButton(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, const char* style, const char* unk) {

    char iniPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, iniPath);
    strcat_s(iniPath, "\\CustomLevels.ini");

    char customName[256];
    const char* finalDisplayText = displayText;

    if (displayText != nullptr && id != nullptr) {

        // Normalize the ID (Converts "LOCKED IMPOSSIBLE" to "14")
        bool isLocked = false;
        const char* lookupId = NormalizeLevelID(id, isLocked);

        // Check the context to pull the right name from the INI
        if (strstr(displayText, "ARENA") != nullptr) {
            GetPrivateProfileStringA(lookupId, "ArenaName", "", customName, sizeof(customName), iniPath);
        }
        else if (strstr(displayText, "RACE") != nullptr) {
            GetPrivateProfileStringA(lookupId, "RaceName", "", customName, sizeof(customName), iniPath);
        }
        else {
            // Fallback for normal buttons
            GetPrivateProfileStringA(lookupId, "Name", "", customName, sizeof(customName), iniPath);
        }

        if (customName[0] != '\0') {
            finalDisplayText = customName;
        }

        // Only apply custom colors if the level is actually unlocked
        if (!isLocked) {
            r = ReadIniFloat(lookupId, "ColorR", r, iniPath);
            g = ReadIniFloat(lookupId, "ColorG", g, iniPath);
            b = ReadIniFloat(lookupId, "ColorB", b, iniPath);
        }
    }

    // Pass everything to the engine
    Original_AddMenuButton(this_ptr, edx_dummy, finalDisplayText, id, vtable, r, g, b, style, unk);
}

const char* __fastcall Hooked_GetLevelName(void* ecx_obj, void* edx_dummy) {

    // The engine stores the Level ID at offset + 8
    int levelID = *(int*)((DWORD)ecx_obj + 8);

    // Safety check: ensure the ID is valid (0 to 15)
    if (levelID < 0 || levelID > 15) {
        return Original_GetLevelName(ecx_obj, edx_dummy);
    }

    char idStr[16];
    sprintf_s(idStr, "%d", levelID);

    // STATIC 2D Array: 16 slots, 256 characters each. 
    // This ensures the pointers we return never get destroyed
    static char customNames[16][256];

    char iniPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, iniPath);
    strcat_s(iniPath, "\\CustomLevels.ini");

    // Try to read the custom name
    GetPrivateProfileStringA(idStr, "RaceName", "", customNames[levelID], 256, iniPath);

    // If we found one, return our custom permanent pointer
    if (customNames[levelID][0] != '\0') {
        return customNames[levelID]; // Return our custom string
    }

    // Otherwise, return the original game's string
    return Original_GetLevelName(ecx_obj, edx_dummy);
}

// Intercept when the game tries to draw the level name when you start a level
void __fastcall Hooked_DrawHUDText(void* this_ptr, void* edx_dummy, const char* text, int x, int y, int shadowOffsetX, int shadowOffsetY, DWORD c1_vtable, float c1_r, float c1_g, float c1_b, float c1_a, DWORD c2_vtable, float c2_r, float c2_g, float c2_b, float c2_a) {

    const char* finalDisplayText = text;
    static char customHUDNames[16][256];

    if (text != nullptr) {

        // Let our helper figure out the ID and the Context
        bool isArena = false;
        const char* levelId = GetLevelIdFromHUDText(text, isArena);

        if (levelId != nullptr) {
            char iniPath[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, iniPath);
            strcat_s(iniPath, "\\CustomLevels.ini");

            int idInt = atoi(levelId);

            // Dynamically select the INI key based on what the engine is trying to draw
            const char* iniKey = isArena ? "ArenaName" : "RaceName";

            // Get the custom name
            GetPrivateProfileStringA(levelId, iniKey, "", customHUDNames[idInt], 256, iniPath);

            // If a custom name exists, swap the pointer
            if (customHUDNames[idInt][0] != '\0') {
                finalDisplayText = customHUDNames[idInt];
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

    char iniPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, iniPath);
    strcat_s(iniPath, "\\CustomLevels.ini");

    // Intercept the "Menu Body" Color (Pure Blue: 0.0, 0.0, 1.0)
    if (r == 0.0f && g == 0.0f && b == 1.0f && a == 0.75f) {
        
        r = ReadIniFloat("GlobalTheme", "MenuBodyR", 0.0f, iniPath);
        g = ReadIniFloat("GlobalTheme", "MenuBodyG", 0.0f, iniPath);
        b = ReadIniFloat("GlobalTheme", "MenuBodyB", 1.0f, iniPath);
        a = ReadIniFloat("GlobalTheme", "MenuBodyA", 0.75f, iniPath);
    }

    // Intercept the "Menu Header" Color (Cyan: 0.0, 1.0, 1.0)
    else if (r == 0.5f && g == 0.5f && b == 1.0f && a == 0.75f) {

        r = ReadIniFloat("GlobalTheme", "MenuHeaderR", 0.5f, iniPath);
        g = ReadIniFloat("GlobalTheme", "MenuHeaderG", 0.5f, iniPath);
        b = ReadIniFloat("GlobalTheme", "MenuHeaderB", 1.0f, iniPath);
        a = ReadIniFloat("GlobalTheme", "MenuHeaderA", 0.75f, iniPath);
    }

    // Pass the filtered color to the actual engine
    return Original_CreateColor(colorStruct, edx_dummy, r, g, b, a);
}