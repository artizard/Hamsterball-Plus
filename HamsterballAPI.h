#pragma once
#include <windows.h>
#include <math.h>

struct Color {
	float r, g, b, a;
	// default (white)
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	// custom color select constructor
	Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
};

struct App {
	// unverified means i don't know for sure that memory address is what i think it is
	void** vtable; // +0x000
	std::uint8_t pad_00C[0x154];
	bool isFullscreen; // +0x158
	bool quitFlag; // +0x159
	bool isGameFocused; // +0x15A
	std::uint8_t pad_15B[0x1];
	int gameWidth; // +0x15C 
	int gameHeight; // +0x160 
	std::uint8_t pad_164[0x10]; 
	void* graphics; // +0x174 unverified
	void* currentScene; // +0x178 unverified; scene object 
	void* audioSystem; // +0x17C unverified
	void* inputHandler; // +0x180 unverified
	void* gameUpdateObj; // +0x184 unverified
	std::uint8_t pad_188[0x238 - 0x188];
	bool rightButtonPauseEnabled; // +0x238
	std::uint8_t pad_23C[0x534 - 0x23C];
	void* musicHandle; // +0x534 unverified
	void* musicChannel1; // +0x538 unverified
	void* musicChannel2; // +0x53C unverified
	std::uint8_t pad540[0x10];

	// take these ones with a massive grain of salt, i'm just mapping them out for good measure
	void* gameMode1; // +0x550 unverified; something about 1 player mode object
	void* gameMode2; // +0x554 unverified; something about 2 player mode object
	void* gameMode3; // +0x558 unverified; something about 4 player mode object
	void* gameMode4; // +0x55C unverified; something about tournament mode object

	std::uint8_t pad_560[0x84C - 0x560];
	float sensitivity; // +0x84C
	bool unlockMirrorTournament; // +0x850
	bool unlockDizzyRace;       // +0x851
	bool unlockTowerRace;       // +0x852
	bool unlockUpRace;          // +0x853
	bool unlockExpertRace;      // +0x854
	bool unlockOddRace;         // +0x855
	bool unlockToobRace;        // +0x856
	bool unlockWobblyRace;      // +0x857
	bool unlockSkyRace;         // +0x858
	bool unlockMasterRace;      // +0x859
	bool unlockDizzyArena;      // +0x85A
	bool unlockTowerArena;      // +0x85B
	bool unlockUpArena;         // +0x85C
	bool unlockExpertArena;     // +0x85D
	bool unlockOddArena;        // +0x85E
	bool unlockToobArena;       // +0x85F
	bool unlockWobblyArena;     // +0x860
	bool unlockSkyArena;        // +0x861
	bool unlockMasterArena;     // +0x862
	bool unlockNeonRace;        // +0x863
	bool unlockGlassRace;       // +0x864
	bool unlockImpossibleRace;  // +0x865
	bool unlockNeonArena;       // +0x866
	bool unlockGlassArena;      // +0x867
	bool unlockImpossibleArena; // +0x868

	// I don't know exactly how they are stored, but these are in the registry
	// I'm sure someone has already figured these out. 
	std::uint8_t bestTimes[0x50]; // +0x86C
	std::uint8_t medals[0x50]; // +0x8BC

	std::uint8_t pad_90C[0xB28 - 0x90C];

	// I don't really know what these do, but they are in the registry 
	DWORD p2Controller1; // +0xB28
	DWORD p2Controller2; // +0xB2C
	DWORD p2Controller3; // +0xB30
	DWORD p2Controller4; // +0xB34

	// there are definitely some more, but i don't know if there is anything useful
};

class IModAPI {
public:
	virtual ~IModAPI() {}

	virtual DWORD GetGameBaseAddress() = 0;
	virtual void RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) = 0;
	virtual bool IsKeyDown(int dik) = 0;
	virtual bool WasKeyPressed(int dik) = 0;
	virtual bool WasKeyReleased(int dik) = 0;
	virtual void CreateToggleButton(const char* id, const char* displayText, bool defaultState, Color color = Color()) = 0;
	virtual bool GetButtonState(const char* id) = 0;
	virtual void* GetPlayer() = 0;
	virtual void PatchMemory(DWORD address, const char* bytes, size_t size) = 0;
};

class HamsterballAPI {
public:
	virtual ~HamsterballAPI() {}
	virtual const char* GetModName() = 0;
	virtual void Initialize(IModAPI* loader) {}
	virtual void onPlayerUpdate(void* PlayerObject) {}
	virtual void onRenderApply(void* this_ptr, float* viewMatrix) {}
	virtual void onButtonToggle(const char* buttonId, bool newState) {}
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
