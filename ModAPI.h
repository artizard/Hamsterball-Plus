#pragma once
#include "HamsterballAPI.h"
#include <string>
#include <map>

struct ButtonData {
	std::string displayText;
	bool isOn;
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
};

extern ModAPI g_ModApiInstance;