#include "pch.h"
#include "Hooks.h"
#include "GameEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include "InputManager.h"
#include "ModAPI.h"
#include <vector>
#include "UniversalObjects.h"
#include <map>
#include <iomanip>
#include "InitHelpers.h"

ThemeConfig g_Theme;
std::vector<LevelConfig> g_LevelConfigs = {};
bool g_ShowCheats = true;
bool g_ShowConsole = false;

const std::string sliderToDisplayText(const SliderData& data) {
    std::stringstream ss;
    ss << data.displayText << ": " << std::fixed << std::setprecision(data.decimalPlaces) << data.value << " " << data.unitName;
    return ss.str();
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
void __fastcall Hooked_PlayerUpdate(Ball* ecx_player, void* edx_dummy) {

    for (HamsterballAPI* mod : g_Mods) {
        mod->onPlayerUpdate((Ball*)ecx_player);
    }
    if (ecx_player->playerID == 0) {
        g_Player = ecx_player; 
        UpdateBallReferences();

        // respawn logic
        if (WasKeyPressed(0x2D)) {
            if (g_Player != nullptr) {
                Original_FindRespawn(g_Player, nullptr);
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
            std::string displayText = data.displayText + ": " + (data.isOn ? data.trueText : data.falseText);
            float r = data.color.r;
            float g = data.color.g;
            float b = data.color.b;
            float a = data.color.a;
            Original_AddMenuButton(this_ptr, nullptr, displayText.c_str(), id.c_str(), vtableAddr, r, g, b, a, nullptr);
        }
        for (const auto& [id, data] : g_ModApiInstance.optionSliders) {
            std::string displayText = sliderToDisplayText(data); 
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
        if (g_AvailableResolutions.empty()) return;

        // Get direct pointers to the width and height stored in the menu object
        int* currentWidth = (int*)((DWORD)this_ptr + 0xCE4);
        int* currentHeight = (int*)((DWORD)this_ptr + 0xCE8);
        Resolution currentRes = { *currentWidth, *currentHeight }; 

        auto it = std::find(g_AvailableResolutions.begin(), g_AvailableResolutions.end(), currentRes);

        // Custom resolution cycle
        if (it != g_AvailableResolutions.end() && std::next(it) != g_AvailableResolutions.end()) {
            *currentWidth = std::next(it)->width;
            *currentHeight = std::next(it)->height; 
        }
        else {
            *currentWidth = g_AvailableResolutions.begin()->width;
            *currentHeight = g_AvailableResolutions.begin()->height; 
        }

        // Format button text
        float ratio = (float)*currentWidth / (float)*currentHeight; 
        const char* aspectRatio = (std::abs(ratio - 1.333f) < .02f) ? "[4:3]" : (std::abs(ratio - 1.778f) < .02f) ? "[16:9]" : (std::abs(ratio - 1.6f) < .02f) ? "[16:10]" : (std::abs(ratio - 1.25f) < .02f) ? "[4:5]" : "";
        char newResText[256];
        sprintf_s(newResText, "RESOLUTION: %d X %d %s", *currentWidth, *currentHeight, aspectRatio);

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

void UpdateBallReferences() {
    if (g_Player) {
        g_Enemies.clear(); // clear list so you don't keep adding to it 
        Scene* scene = g_Player->scene;
        int ball_count = scene->ball_list_count;
        
        for (int i = 0; i < ball_count; i++) {
            Ball* curr_ball = scene->ball_array[i];
            int curr_id = curr_ball->playerID;
            // stolen player 1 should already be updated 
            switch (curr_id) {
            case -1:
                g_Enemies.push_back(curr_ball);
                break;
            case 1:
                g_Player2 = curr_ball;
                break;
            case 2:
                g_Player3 = curr_ball;
                break;
            case 3:
                g_Player4 = curr_ball;
                break;
            }
        }
    }
}

void __fastcall Hooked_CollisionCheck(void* this_ptr, void* edx_dummy, Ball* colliding_ball, int* param_1) {
    BaseCollideCheck(this_ptr, edx_dummy, colliding_ball, param_1); // universal objects thing 
    char* eventPlaneID = (*(char**)(param_1[1] + 0x864));
    for (HamsterballAPI* mod : g_Mods) {
        mod->onEventPlaneCollide(colliding_ball, eventPlaneID);
    }
    Original_CollisionCheck(this_ptr, colliding_ball, param_1); 
}

void __fastcall Hooked_SliderOptionHandler(void* this_ptr, void* edx_dummy, char* sliderID, int inputDirection) {
    std::string currID(sliderID);

    auto it = g_ModApiInstance.optionSliders.find(currID);
    if (it != g_ModApiInstance.optionSliders.end()) {
        auto& data = it->second; 
        data.value += data.stepSize * inputDirection;

        // clamps
        if (data.value < data.lowerBound) {
            data.value = data.lowerBound;
        }
        else if (data.value > data.upperBound) {
            data.value = data.upperBound; 
        }
        // update text in game
        std::string displayText = sliderToDisplayText(data);
        Game_UpdateButtonText(this_ptr, nullptr, displayText.c_str(), currID.c_str());
        // apply logic to mod
        data.owner->onSliderChange(sliderID, data.value);
    }

    Original_SliderOptionHandler(this_ptr, sliderID, inputDirection);
}

void __fastcall Hooked_SaveConfig(App* app) {
    SaveCustomOptions(); 
    Original_SaveConfig(app);
}
