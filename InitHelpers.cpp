#include "pch.h"
#include "InitHelpers.h"
#include <string>
#include "GameEngine.h"
#include <algorithm>
#include <set>

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

void ControlsINI(const char* path) {
    // load/store custom controls
    for (auto& [key, value] : g_CustomControls) {
        UINT curr = GetPrivateProfileIntA("Custom Controls", key.c_str(), 0xFFFFFFFF, path);
        if (curr == 0xFFFFFFFF) { // not in main controls
            UINT backup = GetPrivateProfileIntA("Unused Controls", key.c_str(), 0xFFFFFFFF, path);
            if (backup == 0xFFFFFFFF) { // not in backup either, so write to controls
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
}

void CleanCustomOptions() {
    const char* path = GetModIniPath();
    // toggle buttons
    std::vector<char> sectionBuffer(32768);
    DWORD bytesRead = GetPrivateProfileSectionA("Toggle Buttons", sectionBuffer.data(), 32768, path);
    if (bytesRead > 0) {
        char* currentString = sectionBuffer.data();
        while (*currentString != '\0') {
            std::string entry(currentString);
            size_t eqPos = entry.find('=');
            if (eqPos != std::string::npos) {
                std::string key = entry.substr(0, eqPos);
                std::string value = entry.substr(eqPos + 1);
                if (g_ModApiInstance.optionButtons.find(key) == g_ModApiInstance.optionButtons.end()) {
                    WritePrivateProfileStringA("Unused Toggle Buttons", key.c_str(), value.c_str(), path); 
                    WritePrivateProfileStringA("Toggle Buttons", key.c_str(), NULL, path); 
                }
            }
            currentString += entry.length() + 1;
        }
    }
    // sliders
    bytesRead = GetPrivateProfileSectionA("Sliders", sectionBuffer.data(), 32768, path);
    if (bytesRead > 0) {
        char* currentString = sectionBuffer.data();
        while (*currentString != '\0') {
            std::string entry(currentString);
            size_t eqPos = entry.find('=');
            if (eqPos != std::string::npos) {
                std::string key = entry.substr(0, eqPos);
                std::string value = entry.substr(eqPos + 1);
                if (g_ModApiInstance.optionSliders.find(key) == g_ModApiInstance.optionSliders.end()) {
                    WritePrivateProfileStringA("Unused Sliders", key.c_str(), value.c_str(), path); 
                    WritePrivateProfileStringA("Sliders", key.c_str(), NULL, path); 
                }
            }
            currentString += entry.length() + 1;
        }
    }
}

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

    ControlsINI(path);
    CleanCustomOptions(); 

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

bool ReadToggleButtonIni(const char* id, bool defaultValue) {
    const char* path = GetModIniPath();
    UINT iniValue = GetPrivateProfileIntA("Toggle Buttons", id, 0xFFFFFFFF, path);
    if (iniValue == 0xFFFFFFFF) { // not in main section
        UINT backup = GetPrivateProfileIntA("Unused Toggle Buttons", id, 0xFFFFFFFF, path);
        if (backup == 0xFFFFFFFF) { // not in backup either, so write to main section
            WritePrivateProfileStringA("Toggle Buttons", id, defaultValue ? "1" : "0", path);
            return defaultValue;
        }
        else { // in backup so we move to main section
            bool value = backup != 0;
            WritePrivateProfileStringA("Toggle Buttons", id, value ? "1" : "0", path);
            WritePrivateProfileStringA("Unused Toggle Buttons", id, NULL, path); // remove from unused 
            return value;
        }
    }
    else { // found, so just read in value
        return iniValue != 0; 
    }
}

float ReadSliderIni(const char* id, float defaultValue) {
    const char* path = GetModIniPath();
    char buffer[256];
    GetPrivateProfileStringA("Sliders", id, "_NOT_FOUND", buffer, sizeof(buffer), path);
    std::string mainResult(buffer);
    if (mainResult == "_NOT_FOUND") { // not in main section
        GetPrivateProfileStringA("Unused Sliders", id, "_NOT_FOUND", buffer, sizeof(buffer), path);
        std::string backupResult(buffer);
        if (backupResult == "_NOT_FOUND") { // not in backup either, so write to main section
            WritePrivateProfileStringA("Sliders", id, std::to_string(defaultValue).c_str(), path);
            return defaultValue;
        }
        else { // in backup so we move to main section
            float value;
            try {
                value = std::stof(backupResult);
            }
            catch (...) {
                value = defaultValue; 
            }
            WritePrivateProfileStringA("Sliders", id, std::to_string(value).c_str(), path);
            WritePrivateProfileStringA("Unused Sliders", id, NULL, path); // remove from unused 
            return value;
        }
    }
    else { // found, so just read in value
        try {
            return std::stof(mainResult);
        }
        catch (...) { // invalid float 
            return defaultValue; 
        }
    }
}

void SaveCustomOptions() {
    const char* path = GetModIniPath();
    for (const auto& [key, val] : g_ModApiInstance.optionButtons) {
        WritePrivateProfileStringA("Toggle Buttons", key.c_str(), val.isOn ? "1" : "0", path);
    }
    for (const auto& [key, val] : g_ModApiInstance.optionSliders) {
        WritePrivateProfileStringA("Sliders", key.c_str(), std::to_string(val.value).c_str(), path);
    }
}

void InitResolutions() {
    // i'm filtering out the weird resolutions that no one would conceivably use, so the ones i'm leaving in are:
    std::set<Resolution> allowedResolutions = {
        {800, 600},
        {1024, 768},
        {960, 720},
        {1280, 720},
        {1280, 1024},
        {1440, 1080}, 
        {1920, 1080},
        {1920, 1440},
        {2560, 1440},
        {2880, 2160},
        {3840, 2160}
    };
    DEVMODE devMode;
    ZeroMemory(&devMode, sizeof(DEVMODE)); 
    devMode.dmSize = sizeof(DEVMODE); 

    // In the case that the player has a resolution outside of the default ones, I'm adding their max resolution plus the 4:3 version of that
    if (EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devMode)) { 
        int nativeWidth = devMode.dmPelsWidth;
        int nativeHeight = devMode.dmPelsHeight;
        g_AvailableResolutions.push_back({nativeWidth, nativeHeight});
        int fourByThreeWidth = nativeHeight * 4 / 3;
        g_AvailableResolutions.push_back({ fourByThreeWidth, nativeHeight });
    }
    int i = 0;
    while (EnumDisplaySettings(NULL, i++, &devMode)) {
        auto it = allowedResolutions.find(Resolution{ (int)devMode.dmPelsWidth, (int)devMode.dmPelsHeight }); 
        if (it != allowedResolutions.end()) {
            g_AvailableResolutions.push_back({ (int)devMode.dmPelsWidth, (int)devMode.dmPelsHeight });
        }
    }
    std::sort(g_AvailableResolutions.begin(), g_AvailableResolutions.end()); 
    g_AvailableResolutions.erase(std::unique(g_AvailableResolutions.begin(), g_AvailableResolutions.end()), g_AvailableResolutions.end());
}