#pragma once
#include <map>
#include "ModAPI.h"

const char* GetModIniPath();
float ReadIniFloat(const char* section, const char* key, float defaultValue, const char* filePath); 
void ReloadINI();
void InitDevConsole(); 
void SaveCustomOptions();
bool ReadToggleButtonIni(const char* id, bool defaultValue);
float ReadSliderIni(const char* id, float defaultValue);
void InitResolutions();