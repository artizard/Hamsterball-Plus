#include "pch.h"
#include "InitHelpers.h"
#include <string>
#include "GameEngine.h"

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

    // Menu Colors
    g_Theme.MenuBodyR = ReadIniFloat("Theme", "MenuBodyR", 0.0f, path);
    g_Theme.MenuBodyG = ReadIniFloat("Theme", "MenuBodyG", 0.0f, path);
    g_Theme.MenuBodyB = ReadIniFloat("Theme", "MenuBodyB", 1.0f, path);
    g_Theme.MenuBodyA = ReadIniFloat("Theme", "MenuBodyA", 0.75f, path);

    g_Theme.MenuHeaderR = ReadIniFloat("Theme", "MenuHeaderR", 0.5f, path);
    g_Theme.MenuHeaderG = ReadIniFloat("Theme", "MenuHeaderG", 0.5f, path);
    g_Theme.MenuHeaderB = ReadIniFloat("Theme", "MenuHeaderB", 1.0f, path);
    g_Theme.MenuHeaderA = ReadIniFloat("Theme", "MenuHeaderA", 0.75f, path);

    g_ShowCheats = GetPrivateProfileIntA("Config", "ShowCheats", 1, path) != 0;

    // load/store custom controls
    for (auto& [key, value] : g_CustomControls) {
        UINT curr = GetPrivateProfileIntA("Custom Controls", key.c_str(), 0xFFFFFFFF, path);
        if (curr == -1) { // not in main controls
            UINT backup = GetPrivateProfileIntA("Unused Controls", key.c_str(), 0xFFFFFFFF, path);
            if (backup == -1) { // not in backup either, so write to controls
                char hexBuffer[8];
                sprintf_s(hexBuffer, "0x%02X", value);
                WritePrivateProfileStringA("Custom Controls", key.c_str(), hexBuffer, path);
            }
            else { // in backup so we move to custom controls
                value = backup;
                char hexBuffer[8];
                sprintf_s(hexBuffer, "0x%02X", value);
                WritePrivateProfileStringA("Custom Controls", key.c_str(), hexBuffer, path);
                WritePrivateProfileStringA("Unused Controls", key.c_str(), NULL, path); // remove from unused 
            }

        }
        else { // found, so just read in value
            value = curr;
        }
    }
    // move unused controls to the unused section; I decided on doing this, that way the main controls section doesn't get overly
    // bloated if the user installs and uninstalls a bunch of mods. This way, if they uninstall and reinstall a mod, they will not 
    // lose their custom keybinds.
    std::vector<char> sectionBuffer(32768);
    DWORD bytesRead = GetPrivateProfileSectionA("Custom Controls", sectionBuffer.data(), 32768, path);
    if (bytesRead > 0) {
        char* currentString = sectionBuffer.data();
        while (*currentString != '\0') {
            std::string entry(currentString);
            size_t eqPos = entry.find('=');
            if (eqPos != std::string::npos) {
                std::string key = entry.substr(0, eqPos);
                std::string value = entry.substr(eqPos + 1);
                // if control is in section, but not actually used, then move to unused section
                if (g_CustomControls.find(key) == g_CustomControls.end()) {
                    WritePrivateProfileStringA("Unused Controls", key.c_str(), value.c_str(), path); // add to unused
                    WritePrivateProfileStringA("Custom Controls", key.c_str(), NULL, path); // remove from custom controls
                }
            }
            currentString += entry.length() + 1;
        }
    }


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

void InitDevConsole() {
    const char* path = GetModIniPath();
    g_ShowConsole = GetPrivateProfileIntA("Config", "ShowConsole", 0, path) != 0;
    if (g_ShowConsole) {
        // Spawn command prompt window
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        printf("Hamsterball Mod Debug Console\n");
    }
}