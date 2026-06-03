#pragma once
#include <windows.h>

struct KeyboardState {
	BYTE current[256]{};
	BYTE previous[256]{};
};

extern KeyboardState g_Keyboard;

void UpdateKeyboard(BYTE* gameKeys);
bool IsKeyDown(int dik);
bool WasKeyPressed(int dik);
bool WasKeyReleased(int dik);

