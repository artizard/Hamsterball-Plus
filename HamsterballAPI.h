#pragma once
#include <windows.h>
class IModAPI {
public:
	virtual ~IModAPI() {}

	virtual DWORD GetGameBaseAddress() = 0;
	virtual bool RegisterDynamicHook(DWORD targetAddres, void* detour, void** original) = 0;
	virtual bool IsKeyDown(int dik) = 0;
	virtual bool WasKeyPressed(int dik) = 0;
	virtual bool WasKeyReleased(int dik) = 0;

};

class HamsterballAPI {
public:
	virtual ~HamsterballAPI() {}

	virtual const char* GetModName() = 0;

	virtual void Initialize(IModAPI* loader) {}

	virtual void onPlayerUpdate(void* PlayerObject) {}
};

typedef HamsterballAPI* (*CreateModFunct)();