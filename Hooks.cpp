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