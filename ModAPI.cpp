#include "pch.h"
#include "ModApi.h"
#include "InputManager.h"
#include "MinHook.h"

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


