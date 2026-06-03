#pragma once
#include "HamsterballAPI.h"
#include <string>
#include <map>

struct ButtonData {
	std::string displayText;
	bool isOn;
};

class ModAPI : public IModAPI {
public:
	std::map<std::string, ButtonData> optionButtons; 

	DWORD GetGameBaseAddress() override;
	bool RegisterDynamicHook(DWORD targetAddres, void* detour, void** original) override;
	bool IsKeyDown(int dik) override;
	bool WasKeyPressed(int dik) override;
	bool WasKeyReleased(int dik) override;
	void CreateToggleButton(const char* id, const char* displayText, bool defaultState) override;
	bool GetButtonState(const char* id) override;
	void* GetPlayer() override;
};

extern ModAPI g_ModApiInstance;