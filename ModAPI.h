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

	DWORD GetGameBaseAddress() override;
	void RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) override;
	bool IsKeyDown(int dik) override;
	bool WasKeyPressed(int dik) override;
	bool WasKeyReleased(int dik) override;
	void CreateToggleButton(const char* id, const char* displayText, bool defaultState, Color color) override;
	bool GetButtonState(const char* id) override;
	void* GetPlayer() override;
	void PatchMemory(DWORD address, const char* bytes, size_t size) override;
	App* GetApp() override;
	void UnlockAll() override;
	void LockAll() override;
	bool QuitGame() override;
	bool SaveConfig() override;
	void ApplyForce(void* player, float x, float y, float z, float magnitude) override;
private:
	void setUnlocks(bool isUnlock);
};

extern ModAPI g_ModApiInstance;