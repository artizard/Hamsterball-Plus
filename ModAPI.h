#pragma once
#include "HamsterballAPI.h"

class ModAPI : public IModAPI {
public:
	DWORD GetGameBaseAddress() override;
	bool RegisterDynamicHook(DWORD targetAddres, void* detour, void** original) override;
	bool IsKeyDown(int dik) override;
	bool WasKeyPressed(int dik) override;
	bool WasKeyReleased(int dik) override;
};