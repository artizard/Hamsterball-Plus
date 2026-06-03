#include "pch.h"
#include "InputManager.h"
#include <cstring>

KeyboardState g_Keyboard;

void UpdateKeyboard(BYTE* gameKeys) {
	memcpy(g_Keyboard.previous, g_Keyboard.current, 256);
	memcpy(g_Keyboard.current, gameKeys, 256);
}
bool IsKeyDown(int dik) {
	if (dik < 0 || dik >= 256)
		return false;
	return (g_Keyboard.current[dik] & 0x80) != 0;
}

bool WasKeyPressed(int dik) {
	if (dik < 0 || dik >= 256)
		return false;
	return ((g_Keyboard.current[dik] & 0x80) != 0) && ((g_Keyboard.previous[dik] & 0x80) == 0);
}

bool WasKeyReleased(int dik) {
	if (dik < 0 || dik >= 256)
		return false;
	return ((g_Keyboard.current[dik] & 0x80) == 0) && ((g_Keyboard.previous[dik] & 0x80) != 0);
}