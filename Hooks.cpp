#include "pch.h"
#include "Hooks.h"
#include "GameEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "InputManager.h"
#include "ModAPI.h"

ThemeConfig g_Theme;
std::vector<LevelConfig> g_LevelConfigs = {};
bool g_ShowCheats = true;
bool g_ShowConsole = false;

// --- HELPER FUNCTIONS ---

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

    g_Theme.MenuHeaderR = ReadIniFloat("Theme", "MenuHeaderR", 0.5f, path);
    g_Theme.MenuHeaderG = ReadIniFloat("Theme", "MenuHeaderG", 0.5f, path);
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

        config.BlotR = ReadIniFloat(iStr, "BlotR", -1.f, path);
        config.BlotG = ReadIniFloat(iStr, "BlotG", -1.f, path);
        config.BlotB = ReadIniFloat(iStr, "BlotB", -1.f, path);

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

// --- HOOK IMPLEMENTATIONS ---

// Hooked Player Update Loop (for getting player object pointer)
void __fastcall Hooked_PlayerUpdate(void* ecx_player, void* edx_dummy) {

    // Look at offset 0x18 to check the "Controller ID"
    int* controllerID = (int*)((DWORD)ecx_player + 0x18);

    // ONLY steal the pointer if it is Player 1 (ID = 0)
    // This will safely ignore the 8-Ball (which is likely -1)
    if (*controllerID == 0) {
        g_StolenPlayer = ecx_player;

        for (HamsterballAPI* mod : g_Mods) {
            mod->onPlayerUpdate(ecx_player);
        }

        if (WasKeyPressed(0x2D)) {
            if (g_StolenPlayer != nullptr) {
                // Call the original death function using our passively stolen pointer
                Original_FindRespawn(g_StolenPlayer, nullptr);
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

        for (const auto& [id, data] : g_ModApiInstance.optionButtons) {
            std::string displayText = data.displayText + (data.isOn ? ": YES" : ": NO");
            float r = data.color.r;
            float g = data.color.g;
            float b = data.color.b;
            float a = data.color.a;
            Original_AddMenuButton(this_ptr, nullptr, displayText.c_str(), id.c_str(), vtableAddr, r, g, b, a, nullptr);
        }
    }

    // Return the saved pointer
    return menuPointer;
}

// Logic for clicking options menu buttons
void __fastcall Hooked_OptionsClick(void* this_ptr, void* edx_dummy, const char* clicked_id) {
    std::string id(clicked_id);

    if (g_ModApiInstance.optionButtons.find(id) != g_ModApiInstance.optionButtons.end()) {
        bool newState = !g_ModApiInstance.optionButtons[id].isOn;
        g_ModApiInstance.optionButtons[id].isOn = newState;
        std::string displayText = g_ModApiInstance.optionButtons[id].displayText + (newState ? ": YES" : ": NO");
        Game_UpdateButtonText(this_ptr, nullptr, displayText.c_str(), clicked_id);
        
        for (HamsterballAPI* mod : g_Mods) {
            mod->onButtonToggle(id.c_str(), newState);
        }
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
void __fastcall Hooked_AddMenuButton(void* this_ptr, void* edx_dummy, const char* displayText, const char* id, DWORD vtable, float r, float g, float b, float a, const void* img) {
    /*std::ofstream logFile("C:\\Users\\artiz\\Documents\\loader_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Button Text: " << (displayText ? displayText : "[NULL]") << "\n";
        logFile << "Image hex:     0x" << std::hex << (DWORD)img << std::dec << "\n";
        logFile << "Vtable hex:     0x" << std::hex << (DWORD)vtable << std::dec << "\n";
        logFile << "r:" << r << std::dec << "\n";
        logFile << "g:" << g << std::dec << "\n";
        logFile << "b:" << b << std::dec << "\n";
        logFile << "a:" << a << std::dec << "\n";
        logFile << "-----------------------------------------\n";
        logFile.close();
    }*/

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
    Original_AddMenuButton(this_ptr, edx_dummy, finalDisplayText, id, vtable, r, g, b, a, img);
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

// Override timer blot color
void __fastcall Hooked_HudManager(void* this_ptr, void* edx_dummy, void* param_1) {

    // Grab the current level name string from the GameState object
    char* levelNamePtr = *(char**)((DWORD)this_ptr + 0x29b4);

    if (levelNamePtr != nullptr) {
        // Convert to ID
        bool isArena = false;
        int levelID = GetLevelIdFromHUDText(levelNamePtr, isArena);

        // Safety Check
        if (levelID >= 0 && levelID < (int)g_LevelConfigs.size()) {

            // Grab our custom blot colors from the INI
            float customR = g_LevelConfigs[levelID].BlotR;
            float customG = g_LevelConfigs[levelID].BlotG;
            float customB = g_LevelConfigs[levelID].BlotB;

            // If the INI actually has a custom color set (not -1), overwrite the game's memory
            if (customR != -1.f) {
                float* pBlotR = (float*)((DWORD)this_ptr + 0x1508);
                float* pBlotG = (float*)((DWORD)this_ptr + 0x150c);
                float* pBlotB = (float*)((DWORD)this_ptr + 0x1510);

                *pBlotR = customR;
                *pBlotG = customG;
                *pBlotB = customB;
            }
        }
    }

    // Let the original engine draw the HUD using the colors we just forcefully injected
    Original_HudManager(this_ptr, param_1);
}

// Overrides the Camera Angle
void __fastcall Hooked_RenderApply(void* this_ptr, void* edx_dummy, float* viewMatrix) {

    for (HamsterballAPI* mod : g_Mods) {
        mod->onRenderApply(this_ptr, viewMatrix);
    }

    Original_RenderApply(this_ptr, edx_dummy, viewMatrix);
}

void __fastcall Hooked_PollInputs(void* self) {
    Original_PollInputs(self); 

    BYTE* base = (BYTE*)self;

    BYTE* keyboardObj = *(BYTE**)(base + 0x434);

    BYTE* inputBuffer = keyboardObj + 0x0C;

    UpdateKeyboard(inputBuffer); 

    /*inputBuffer[0xCD] = 0x80;
    bool isShiftDown = (inputBuffer[0x2A] & 0x80) != 0;
    bool isRightDown = (inputBuffer[0xCD] & 0x80) != 0;
    std::ofstream logFile("C:\\Users\\artiz\\Documents\\loader_log.txt", std::ios::app);
    if (logFile.is_open()) {

        logFile << "==== Nonzero Input Buffer Dump ====\n";
        logFile << "Self: 0x" << std::hex << (uintptr_t)self << std::dec << "\n";

        int count = 0;

        for (int i = 0; i < 256; i++)
        {
            if (inputBuffer[i] != 0)
            {
                logFile << "DIK 0x"
                    << std::hex << i
                    << " = 0x"
                    << (int)inputBuffer[i]
                    << std::dec
                    << "\n";

                count++;
            }
        }

        logFile << "Nonzero count: " << count << "\n";
        logFile << "===================================\n\n";


        logFile.close();
    }*/
    
}

void __fastcall Hooked_GameUpdate(App* app) {
    for (HamsterballAPI* mod : g_Mods) {
        mod->onGameUpdate();
    }
    Original_GameUpdate(app); 
}