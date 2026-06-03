#include "pch.h"
#include "ModApi.h"
#include "InputManager.h"
#include "MinHook.h"
#include "GameEngine.h"

DWORD ModAPI::GetGameBaseAddress() {
	return (DWORD)GetModuleHandle(NULL);
}

void ModAPI::RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) {
	MH_CreateHook((LPVOID)targetAddress, hookFunction, original);
	MH_EnableHook((LPVOID)targetAddress);
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

void ModAPI::CreateToggleButton(const char* id, const char* displayText, bool defaultState, Color color) {
	ButtonData data;
	data.displayText = displayText;
	data.isOn = defaultState;
	data.color = color;
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

void ModAPI::PatchMemory(DWORD address, const char* bytes, size_t size) {
	DWORD oldProtect;

	// Unlock memory
	VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Copy new bytes
	memcpy((void*)address, bytes, size);

	// Lock up memory again
	VirtualProtect((void*)address, size, oldProtect, &oldProtect);
}