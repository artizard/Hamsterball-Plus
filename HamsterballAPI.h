#pragma once
#include <windows.h>
#include <math.h>

class IModAPI {
public:
	virtual ~IModAPI() {}

	virtual DWORD GetGameBaseAddress() = 0;
	virtual bool RegisterDynamicHook(DWORD targetAddres, void* detour, void** original) = 0;
	virtual bool IsKeyDown(int dik) = 0;
	virtual bool WasKeyPressed(int dik) = 0;
	virtual bool WasKeyReleased(int dik) = 0;
	virtual void CreateToggleButton(const char* id, const char* displayText, bool defaultState) = 0;
	virtual bool GetButtonState(const char* id) = 0;
	virtual void* GetPlayer() = 0;
};

class HamsterballAPI {
public:
	virtual ~HamsterballAPI() {}

	virtual const char* GetModName() = 0;

	virtual void Initialize(IModAPI* loader) {}

	virtual void onPlayerUpdate(void* PlayerObject) {}

	virtual void onRenderApply(void* this_ptr, float* viewMatrix) {}
};

typedef HamsterballAPI* (*CreateModFunct)();

// --- Vector3 ---

// Simple 3D Vector struct
struct Vec3 {
	float x, y, z;
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

// Math Helpers
inline Vec3 Subtract(Vec3 a, Vec3 b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vec3 Cross(Vec3 a, Vec3 b) {
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 Normalize(Vec3 v) {
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0.0f) return Vec3(0, 0, 0);
	return Vec3(v.x / length, v.y / length, v.z / length);
}

// Builds a DirectX 8 compatible View Matrix (16 floats)
inline void BuildCustomViewMatrix(float* outMatrix, Vec3 eye, Vec3 target, Vec3 up) {
	Vec3 zaxis = Normalize(Subtract(target, eye));
	Vec3 xaxis = Normalize(Cross(up, zaxis));
	Vec3 yaxis = Cross(zaxis, xaxis);

	outMatrix[0] = xaxis.x;           outMatrix[1] = yaxis.x;           outMatrix[2] = zaxis.x;           outMatrix[3] = 0.0f;
	outMatrix[4] = xaxis.y;           outMatrix[5] = yaxis.y;           outMatrix[6] = zaxis.y;           outMatrix[7] = 0.0f;
	outMatrix[8] = xaxis.z;           outMatrix[9] = yaxis.z;           outMatrix[10] = zaxis.z;          outMatrix[11] = 0.0f;
	outMatrix[12] = -Dot(xaxis, eye); outMatrix[13] = -Dot(yaxis, eye); outMatrix[14] = -Dot(zaxis, eye); outMatrix[15] = 1.0f;
}