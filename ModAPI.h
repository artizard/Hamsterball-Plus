#pragma once
#include "HamsterballAPI.h"
#include <string>
#include <map>

struct ButtonData {
	std::string displayText;
	bool isOn = false;
	Color color;
};

class ModAPI : public IModAPI {
public:
	std::map<std::string, ButtonData> optionButtons; 

	void RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) override;
	void RegisterCustomControl(const char* controlID, int default_dik) override;
	int GetCustomControlKey(const char* controlID) override;
	bool IsKeyDown(int dik) override;
	bool WasKeyPressed(int dik) override;
	bool WasKeyReleased(int dik) override;
	bool IsControlDown(const char* controlID) override;
	bool WasControlPressed(const char* controlID) override;
	bool WasControlReleased(const char* controlID) override;
	void CreateToggleButton(const char* id, const char* displayText, bool defaultState, Color color) override;
	PhysicsObject* GetPhysicsObj() override;
	void PatchMemory(DWORD address, const char* bytes, size_t size) override;
	void UnlockAll() override;
	void LockAll() override;
	bool QuitGame() override;
	bool SaveConfig() override;
	void ApplyForce(Ball* player, float x, float y, float z, float magnitude) override;
	void SetSpeed(Ball* player, float mult) override;
	
	Ball* GetPlayer() override;
	Ball* GetPlayer2() override;
	Ball* GetPlayer3() override;
	Ball* GetPlayer4() override;
	Ball** GetEnemies(size_t* enemyCount) override;
	DWORD GetGameBaseAddress() override;
	Scene* GetScene() override;
	App* GetApp() override;
	bool GetButtonState(const char* id) override;
	void* AllocateMem(unsigned int size) override;
	void CreateBadBall(Vec3 spawn_pos, Vec3 home_pos, float home_distance, float chase_distance, float radius, float spin_distance) override;
private:
	void setUnlocks(bool isUnlock);
	void ReloadIniFile();
};

extern ModAPI g_ModApiInstance;