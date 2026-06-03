#include "pch.h"
#include "ModApi.h"
#include "InputManager.h"
#include "MinHook.h"
#include "GameEngine.h"

DWORD ModAPI::GetGameBaseAddress() {
	return (DWORD)GetModuleHandle(NULL);
}

bool ModAPI::RegisterDynamicHook(DWORD targetAddres, void* detour, void** original) {
	return true;
}

bool ModAPI::IsKeyDown(int dik) {
	return ::IsKeyDown(dik);
}
bool ModAPI::WasKeyPressed(int dik) {
	return ::WasKeyPressed(dik);
}
bool ModAPI::WasKeyReleased(int dik) {
	return ::WasKeyReleased(dik);
}

void ModAPI::CreateToggleButton(const char* id, const char* displayText, bool defaultState) {
	ButtonData data;
	data.displayText = displayText;
	data.isOn = defaultState;
	optionButtons[std::string(id)] = data;
}

bool ModAPI::GetButtonState(const char* id) {
	std::string idString(id);
	if (optionButtons.find(idString) != optionButtons.end()) {
		return optionButtons[idString].isOn; 
	}
	return false; // failsafe 
}

void* ModAPI::GetPlayer() {
	return g_StolenPlayer; 
}

