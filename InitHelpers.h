#pragma once

const char* GetModIniPath();
float ReadIniFloat(const char* section, const char* key, float defaultValue, const char* filePath); 
void ReloadINI();
void InitDevConsole(); 
void SaveCustomOptions(); 