#include "pch.h"
#include "ModApi.h"
#include "InputManager.h"
#include "MinHook.h"
#include "GameEngine.h"
#include "Hooks.h"
#include "InitHelpers.h"

DWORD ModAPI::GetGameBaseAddress() {
	return (DWORD)GetModuleHandle(NULL);
}

void ModAPI::RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) {
	MH_STATUS createStatus = MH_CreateHook((LPVOID)targetAddress, hookFunction, original);
	MH_STATUS enableStatus = MH_EnableHook((LPVOID)targetAddress);
	if (createStatus != MH_OK) {
		printf("Minhook create error at %x: %s\n", targetAddress, MH_StatusToString(createStatus));
	}
	if (enableStatus != MH_OK) {
		printf("Minhook enable error at %x: %s\n", targetAddress, MH_StatusToString(enableStatus));
	}
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

bool ModAPI::IsControlDown(const char* controlID) {
	int key = GetCustomControlKey(controlID);
	if (key != -1) {
		return IsKeyDown(key);
	}
	else {
		return false; 
	}
}
bool ModAPI::WasControlPressed(const char* controlID) {
	int key = GetCustomControlKey(controlID);
	if (key != -1) {
		return WasKeyPressed(key);
	}
	else {
		return false;
	}
}
bool ModAPI::WasControlReleased(const char* controlID) {
	int key = GetCustomControlKey(controlID);
	if (key != -1) {
		return WasKeyReleased(key);
	}
	else {
		return false;
	}
}

void ModAPI::CreateToggleButton(CustomButton button, HamsterballAPI* modInstance) {
	ButtonData data;
	data.displayText = button.displayText;
	data.isOn = ReadToggleButtonIni(button.id, button.defaultState); 
	data.color = button.color;
	data.trueText = button.trueText;
	data.falseText = button.falseText; 
	data.owner = modInstance; 
	optionButtons[std::string(button.id)] = data;
	modInstance->onButtonToggle(button.id, data.isOn); // update state 
}

void ModAPI::CreateSlider(CustomSlider slider, HamsterballAPI* modInstance) {
	SliderData data; 
	data.displayText = slider.displayText;
	data.value = ReadSliderIni(slider.id, slider.startingValue); 
	data.stepSize = slider.stepSize;
	data.color = slider.color; 
	data.decimalPlaces = slider.decimalPlaces;
	data.lowerBound = slider.lowerBound;
	data.upperBound = slider.upperBound;
	data.unitName = slider.unitName; 
	data.owner = modInstance; 
	optionSliders[std::string(slider.id)] = data; 
	modInstance->onSliderChange(slider.id, data.value); 
}

bool ModAPI::GetButtonState(const char* id) {
	std::string idString(id);
	if (optionButtons.find(idString) != optionButtons.end()) {
		return optionButtons[idString].isOn; 
	}
	return false; // failsafe 
}

float ModAPI::GetSliderState(const char* id) {
	std::string idString(id);
	if (optionSliders.find(idString) != optionSliders.end()) {
		return optionSliders[idString].value;
	}
	return -1.0; 
}

Ball* ModAPI::GetPlayer() {
	return (Ball*) g_Player; 
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
		Original_SaveConfig(g_App); 
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

void ModAPI::ApplyForce(Ball* player, float x, float y, float z, float magnitude) {
	if (player) {
		::ApplyForce(player, x, y, z, magnitude);
	}
}

PhysicsObject* ModAPI::GetPhysicsObj() {
	return (g_Player)->physics_object;
}

void ModAPI::SetSpeed(Ball* player, float mult) {
	PhysicsObject* physics = player->physics_object; 
	Vec3 velocity = Vec3(physics->velocity_x, physics->velocity_y, physics->velocity_z); 
	Vec3 normalized = Normalize(velocity);
	physics->velocity_x = normalized.x * mult;
	physics->velocity_y = normalized.y * mult;
	physics->velocity_z = normalized.z * mult;
}

Scene* ModAPI::GetScene() {
	if (g_Player) {
		return g_Player->scene;
	}
}

Ball* ModAPI::GetPlayer2() {
	if (g_Player2) {
		return g_Player2;
	}
	else {
		return nullptr;
	}
}
Ball* ModAPI::GetPlayer3() {
	if (g_Player3) {
		return g_Player3;
	}
	else {
		return nullptr;
	}
}
Ball* ModAPI::GetPlayer4() {
	if (g_Player4) {
		return g_Player4;
	}
	else {
		return nullptr;
	}
}

// pass in &enemyCount to get the size of the array that is returned 
Ball** ModAPI::GetEnemies(size_t* enemyCount) {
	*enemyCount = g_Enemies.size();
	if (!g_Enemies.empty()) {
		return g_Enemies.data();
	}
	else {
		return nullptr;
	}
}

void* ModAPI::AllocateMem(unsigned int size) {
	HANDLE hCrtHeap = *(HANDLE*)0x005369C0;
	return HeapAlloc(hCrtHeap, 0, size);
}

void ModAPI::CreateBadBall(Vec3 spawn_pos, Vec3 home_pos, float home_distance, float chase_distance, float radius, float spin_distance) {
	void* mem = AllocateMem(0xC98);
	Ball* badball = nullptr;
	Scene* scene = GetScene();
	badball = CallMethod<Ball*>(0x0AFE0, mem, scene); // ball constructor 
	//badball = CallMethod<Ball*>(0x039E0, &badball, api->GetScene()); 
	CallMethod(0x05100, badball); // set physics? without this the physics are all weird, 8ball defies gravity 
	badball->pos_x = spawn_pos.x;
	badball->pos_y = spawn_pos.y;
	badball->pos_z = spawn_pos.z;
	badball->home_position_x = home_pos.x;
	badball->home_position_y = home_pos.y;
	badball->home_position_z = home_pos.z;
	badball->home_distance = home_distance;
	badball->chase_distance = chase_distance;
	badball->radius = radius;
	badball->spin_distance = spin_distance;
	*((uint8_t*)badball + 0x281) = 0; // unsure if this one matters 
	CallMethod(0x53780, &scene->ball_list, badball);  // append ball to ball_list 
	CallMethod(0x53780, (char*)scene + 0x2DEC, badball); // unsure if this is required, but the original function calls this, not removing in case it causes memory leaks or whatever 
}

void ModAPI::RegisterCustomControl(const char* controlID, int default_dik) {
	std::string controlString(controlID); 
	g_CustomControls[controlString] = default_dik; 
}

int ModAPI::GetCustomControlKey(const char* controlID) {
	std::string controlString(controlID);
	auto val = g_CustomControls.find(controlString);
	if (val != g_CustomControls.end()) {
		return val->second; 
	}
	else {
		return -1; // couldn't find it 
	}
}

void ModAPI::ReloadIniFile() {
	ReloadINI(); 
}


int ModAPI::GetTimerTime() {
	return *g_Timer;
}

void ModAPI::SetTimerTime(int time) {
	*g_Timer = time;
}

Vec3 ModAPI::LevelRaycastVec(Vec3 position, Vec3 direction, float max_dist) {
	Collision* collision = GetScene()->collision_mesh;
	Vec3 result = Vec3(0, 0, 0);
	if (!collision) {
		return result;
	}
	LevelRaycast(GetScene()->collision_mesh, &result, position, direction, max_dist);
	return result; 
}

bool ModAPI::LevelRaycastHit(Vec3 position, Vec3 direction, float max_dist, float tolerance) {
	Collision* collision = GetScene()->collision_mesh; 
	if (!collision) {
		return false;
	}
	Vec3 result = Vec3(0, 0, 0);
	LevelRaycast(collision, &result, position, direction, 26.0f);
	float dist_x = result.x - position.x;
	float dist_y = result.y - position.y;
	float dist_z = result.z - position.z;
	float magnitude = sqrtf(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z);
	if (magnitude < max_dist + tolerance) {
		return true;
	} 
	return false;
}

PhysicsConstants* ModAPI::GetPhysicsConstants() {
	return g_PhysicsConstants; 
}

void ModAPI::PlaySoundEffect(void* soundEffect, float volume) {
	::PlaySoundEffect(soundEffect, volume);
}
void ModAPI::Play3dSoundEffect(void* soundEffect, Vec3 position, float volume) {
	::Play3dSoundEffect(soundEffect, position, volume);
}