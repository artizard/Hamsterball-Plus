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

Ball* ModAPI::GetPlayer() {
	return (Ball*) g_StolenPlayer; 
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

App* ModAPI::GetApp() {
	return g_App; 
}
void ModAPI::UnlockAll() {
	setUnlocks(true); 
}
void ModAPI::LockAll() {
	setUnlocks(false); 
}

bool ModAPI::SaveConfig() {
	if (g_App) {
		::SaveConfig(g_App);
		return true;
	}
	else {
		// couldn't save
		return false;
	}
}

// true = unlock all, false = lock all 
void ModAPI::setUnlocks(bool isUnlock) {
	g_App->unlockMirrorTournament = isUnlock;
	g_App->unlockDizzyRace = isUnlock;
	g_App->unlockTowerRace = isUnlock;
	g_App->unlockUpRace = isUnlock;
	g_App->unlockExpertRace = isUnlock;
	g_App->unlockOddRace = isUnlock;
	g_App->unlockToobRace = isUnlock;
	g_App->unlockWobblyRace = isUnlock;
	g_App->unlockSkyRace = isUnlock;
	g_App->unlockMasterRace = isUnlock;
	g_App->unlockDizzyArena = isUnlock;
	g_App->unlockTowerArena = isUnlock;
	g_App->unlockUpArena = isUnlock;
	g_App->unlockExpertArena = isUnlock;
	g_App->unlockOddArena = isUnlock;
	g_App->unlockToobArena = isUnlock;
	g_App->unlockWobblyArena = isUnlock;
	g_App->unlockSkyArena = isUnlock;
	g_App->unlockMasterArena = isUnlock;
	g_App->unlockNeonRace = isUnlock;
	g_App->unlockGlassRace = isUnlock;
	g_App->unlockImpossibleRace = isUnlock;
	g_App->unlockNeonArena = isUnlock;
	g_App->unlockGlassArena = isUnlock;
	g_App->unlockImpossibleArena = isUnlock;
	SaveConfig(); 
}

bool ModAPI::QuitGame() {
	if (g_App) {
		g_App->quitFlag = true;
		return true;
	}
	else {
		// couldn't quit 
		return false; 
	}
}

void ModAPI::ApplyForce(void* player, float x, float y, float z, float magnitude) {
	if (player) {
		::ApplyForce(player, x, y, z, magnitude);
	}
}

PhysicsObject* ModAPI::GetPhysicsObj() {
	return ((Ball*)g_StolenPlayer)->physics_object;
}