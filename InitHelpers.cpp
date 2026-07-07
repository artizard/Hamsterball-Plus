#include "pch.h"
#include "InitHelpers.h"
#include <string>
#include <set>
#include <variant>
#include "HamsterballAPI.h"
#include <algorithm>

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

CustomControl ParseIniControl(std::string controlString) {
    CustomControl control; 
    std::string dikString = controlString;
    if (controlString.find("CTRL+") != std::string::npos) {
        control.requiresCtrl = true;
        dikString = controlString.substr(5);
    }
    try {
        control.dikCode = std::stoi(dikString, nullptr, 16); 
    }
    catch (...) {
        control.dikCode = -1; 
    }
    return control;
}

std::string FormatControlForIni(CustomControl control) {
    char hexBuffer[16];
    if (control.requiresCtrl) {
        sprintf_s(hexBuffer, "CTRL+0x%02X", control.dikCode);
    }
    else {
        sprintf_s(hexBuffer, "0x%02X", control.dikCode);
    }
    return std::string(hexBuffer);
}

// moves unused controls/config/options to the unused section; I decided on doing this, that way the sections don't get overly
// bloated if the user installs and uninstalls a bunch of mods. This way, if they uninstall and reinstall a mod, they will not 
// lose their custom keybinds/configs/etc.
template <typename MapType, typename CompareType>
void CleanSection(const char* mainSection, const char* unusedSection, std::map<std::string, MapType, CompareType>& sectionMap) {
    const char* path = GetModIniPath();
    std::vector<char> sectionBuffer(32768);
    DWORD bytesRead = GetPrivateProfileSectionA(mainSection, sectionBuffer.data(), 32768, path);
    if (bytesRead > 0) {
        char* currentString = sectionBuffer.data();
        while (*currentString != '\0') {
            std::string entry(currentString);
            size_t eqPos = entry.find('=');
            if (eqPos != std::string::npos) {
                std::string key = entry.substr(0, eqPos);
                std::string value = entry.substr(eqPos + 1);
                if (sectionMap.find(key) == sectionMap.end()) {
                    WritePrivateProfileStringA(unusedSection, key.c_str(), value.c_str(), path);
                    WritePrivateProfileStringA(mainSection, key.c_str(), NULL, path);
                }
            }
            currentString += entry.length() + 1;
        }
    }
}

void ControlsINI(const char* path) {
    // load/store custom controls
    char controlBuffer[16];
    for (auto& [key, value] : g_ModApiInstance.customControls) {
        DWORD bytesRead = GetPrivateProfileStringA("Custom Controls", key.c_str(), "", controlBuffer, sizeof(controlBuffer), path);
        if (bytesRead == 0) { // not in main controls
            bytesRead = GetPrivateProfileStringA("Unused Controls", key.c_str(), "", controlBuffer, sizeof(controlBuffer), path);
            if (bytesRead == 0) { // not in backup either, so write to controls
                std::string formatted = FormatControlForIni(value);
                WritePrivateProfileStringA("Custom Controls", key.c_str(), formatted.c_str(), path);
            }
            else { // in backup so we move to custom controls
                CustomControl control = ParseIniControl(controlBuffer);
                if (control.dikCode != -1) value = control;
                std::string formatted = FormatControlForIni(value);
                WritePrivateProfileStringA("Custom Controls", key.c_str(), formatted.c_str(), path);
                WritePrivateProfileStringA("Unused Controls", key.c_str(), NULL, path); // remove from unused 
            }
        }
        else { // found, so just read in value
            CustomControl control = ParseIniControl(controlBuffer);
            if (control.dikCode != -1) value = control;
        }
    }
    CleanSection("Custom Controls", "Unused Controls", g_ModApiInstance.customControls);
}

std::string FormatConfigForIni(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    }
    else if (std::holds_alternative<float>(value)) {
        return std::to_string(std::get<float>(value));
    }
    else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "1" : "0";
    }
    else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return "";
}

void ParseStringToConfig(const std::string& value, ConfigValue& config) {
    try {
        if (std::holds_alternative<int>(config)) {
            config = std::stoi(value, nullptr, 0);
        }
        else if (std::holds_alternative<float>(config)) {
            config = std::stof(value);
        }
        else if (std::holds_alternative<bool>(config)) {
            config = (value == "1" || value == "true" || value == "True"); 
        }
        else if (std::holds_alternative<std::string>(config)) {
            config = value;
        }
    }
    catch (...) {
        printf("ERROR: COULD NOT PARSE INI CONFIG"); 
    }
}

void ConfigINI(const char* path) {
    char controlBuffer[256];
    for (auto& [key, value] : g_ModApiInstance.modConfig) {
        DWORD bytesRead = GetPrivateProfileStringA("Custom Configs", key.c_str(), "", controlBuffer, sizeof(controlBuffer), path);
        if (bytesRead == 0) { // not in main config
            bytesRead = GetPrivateProfileStringA("Unused Configs", key.c_str(), "", controlBuffer, sizeof(controlBuffer), path);
            if (bytesRead == 0) { // not in backup either, so write to config 
                std::string formatted = FormatConfigForIni(value);
                WritePrivateProfileStringA("Custom Configs", key.c_str(), formatted.c_str(), path);
            }
            else { // in backup so we move to custom config
                ParseStringToConfig(controlBuffer, value); 

                std::string formatted = FormatConfigForIni(value);
                WritePrivateProfileStringA("Custom Configs", key.c_str(), formatted.c_str(), path);
                WritePrivateProfileStringA("Unused Configs", key.c_str(), NULL, path); // remove from unused 
            }
        }
        else { // found, so just read in value
            ParseStringToConfig(controlBuffer, value);
        }
    }
    CleanSection("Custom Configs", "Unused Configs", g_ModApiInstance.modConfig);
}

void CleanCustomOptions() {
    
    

    CleanSection("Toggle Buttons", "Unused Toggle Buttons", g_ModApiInstance.optionButtons);
    CleanSection("Sliders", "Unused Sliders", g_ModApiInstance.optionSliders);
    CleanSection("Cycle Options", "Unused Cycle Options", g_ModApiInstance.optionCycles);
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

    // ShowConsole was moved to InitDevConsole() 

    ControlsINI(path);
    CleanCustomOptions(); 
    ConfigINI(path);


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

        char tempRace[256], tempArena[256];
        GetPrivateProfileStringA(iStr, "RaceName", "", tempRace, sizeof(tempRace), path);
        GetPrivateProfileStringA(iStr, "ArenaName", "", tempArena, sizeof(tempArena), path);

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
        printf("Hamsterball Plus Version %d\n", HAMSTERBALL_API_VERSION);
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

int ReadCycleIni(const char* id, int defaultValue) {
    const char* path = GetModIniPath();
    UINT iniValue = GetPrivateProfileIntA("Cycle Options", id, 0xFFFFFFFF, path);
    if (iniValue == 0xFFFFFFFF) { // not in main section
        UINT backup = GetPrivateProfileIntA("Unused Cycle Options", id, 0xFFFFFFFF, path);
        if (backup == 0xFFFFFFFF) { // not in backup either, so write to main section
            std::string val = std::to_string(defaultValue); 
            WritePrivateProfileStringA("Cycle Options", id, val.c_str(), path);
            return defaultValue;
        }
        else { // in backup so we move to main section
            int value = backup;
            std::string backupStr = std::to_string(backup);
            WritePrivateProfileStringA("Cycle Options", id, backupStr.c_str(), path);
            WritePrivateProfileStringA("Unused Cycle Options", id, NULL, path); // remove from unused 
            return value;
        }
    }
    else { // found, so just read in value
        return iniValue;
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
    for (const auto& [key, val] : g_ModApiInstance.optionCycles) {
        WritePrivateProfileStringA("Cycle Options", key.c_str(), std::to_string(val.currOption).c_str(), path);
    }
}

void InitResolutions() {
    // i'm filtering out the weird resolutions that no one would conceivably use, so the ones i'm leaving in are:
    std::set<Resolution> allowedResolutions = {
        {640, 480},
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