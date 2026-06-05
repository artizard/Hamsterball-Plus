#pragma once
#include <windows.h>
#include <math.h>
#include <cstdint>
#include <cstddef>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

struct Color {
	float r, g, b, a;
	// default (white)
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	// custom color select constructor
	Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
};

#pragma pack(push, 1)
struct PhysicsConstants {
    // unverified means i don't know for sure that memory address is what i think it is
	float unknown; // 0x4CF368
	float dizzyForceMult; // 0x4CF36C unverified
	std::uint8_t pad_08[0x04];
	float glassForceMult; // 0x4CF374
	float unknown2; // 0x4CF378
	std::uint8_t pad_14[0x04];
	float unknown3; // 0x4Cf380
	std::uint8_t pad_1C[0x18];
	float hamsterSize; // 0x4CF39C default is .037
	std::uint8_t pad_38[0x48];
	float unknown4; // 0x4CF3E8 not exactly sure what this is, but it affected the 8ball on glass race 
	float cameraDamping; // 0x4CF3F0
	float unknown5; // 0x4CF484
	float unknown6; // 0x4CF48C 
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Ball {
	// unverified means i don't know for sure that memory address is what i think it is
	void** vtable; // +0x000
	std::uint8_t pad_004[0x014 - 0x004];
	void* scene; // +0x014 unverified
	std::int32_t player_index; // +0x018 
	std::uint8_t pad_01C[0x158 - 0x01C];
	float prev_pos_x; // +0x158
	float prev_pos_y; // +0x15C
	float prev_pos_z; // +0x160
	float pos_x; // +0x164
	float pos_y; // +0x168
	float pos_z; // +0x16C
	// there is something weird going on with velocity, so take that with a grain of salt 
	float vel_x; // +0x170
	float vel_y; // +0x174
	float vel_z; // +0x178
	float accel_x; // +0x17C
	float accel_y; // +0x180
	float accel_z; // +0x184
	float max_speed; // +0x188
	std::uint8_t pad_18C[0x1A0 - 0x18C];
	float speed_mult; // +0x1A0
	void* collision_mesh; // +0x1A4 unverified, changing it crashed the game
	float gravity_vec[3]; // +0x1A8 can't change this, it always changes back to default 
	std::uint8_t pad_1B4[0x1C8 - 0x1B4];
	float ball_outline_opacity; // +0x1C8
	std::uint8_t pad_1CC[0x20C - 0x1CC];
	float color_r; // +0x20C unverified 
	float color_g; // +0x210 unverified 
	float color_b; // +0x214 unverified 
	float color_a; // +0x218 unverified 
	std::uint8_t pad_21C[0x260 - 0x21C];
	bool boost_hit_flag; // +0x260 unverified 
	std::uint8_t pad_261[0x264 - 0x261];
	std::uint8_t rumble_timer1[0x14]; // +0x264 unverified
	float bounciness; // +0x278
	std::uint8_t pad_27C[0x284 - 0x27C];
	float radius; // +0x284
	std::uint8_t pad_288[0x290 - 0x288];
	std::uint8_t rumble_timer2[0x14]; // +0x290 unverified
	float spin_rate; // +0x2A4 very janky
	std::uint8_t pad_2A8[0x2BC - 0x2A8]; 
	float force_x; // +0x2BC
	float force_y; // +0x2C0
	float force_z; // +0x2C4
	std::uint8_t pad_2C8[0x2CC - 0x2C8];
	bool disable_ball; // +0x2CC
	std::uint8_t pad_2CD[0x2DC - 0x2CD];
	float checkpoint_x; // +0x2DC
	float checkpoint_y; // +0x2E0
	float checkpoint_z; // +0x2E4
	bool event_checkpoint_flag; // +0x2E8 unverified
	bool unknown; // +0x2E9 i don't know what this does, but it does break the ball if you set it to true
	std::uint8_t pad_2EA[0x310 - 0x2EA];
	bool state_active; // +0x310 unverified
	std::uint8_t pad_311[0x700 - 0x311];
	std::int32_t sound_3d_handle; // +0x700 unverified
	std::uint8_t pad_704[0x768 - 0x704];
	bool cam_active; // +0x768
	std::uint8_t pad_769[0xC4C - 0x769];
	bool low_gravity_mode; // +0xC4C
	std::uint8_t pad_C4D[0xC88 - 0xC4D];
	float world_matrix[16]; // +0xC88 unverified
};
#pragma pack(pop)

#pragma pack(push, 1)
struct App {
	// unverified means i don't know for sure that memory address is what i think it is
	void** vtable; // +0x000
	std::uint8_t pad_004[0x158 - 0x004];
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
	std::uint8_t pad_239[0x534 - 0x239];
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

	std::uint8_t pad_869[0x86C - 0x869]; 

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
#pragma pack(pop)

// this makes sure the offsets are right 
static_assert(sizeof(void*) == 4, "void* wasnt 4 bytes");
static_assert(sizeof(bool) == 1, "bool wasnt 1 byte");
static_assert(sizeof(DWORD) == 4, "DWORD wasnt 4 bytes");
static_assert(offsetof(App, vtable) == 0x000);
static_assert(offsetof(App, isFullscreen) == 0x158);
static_assert(offsetof(App, quitFlag) == 0x159);
static_assert(offsetof(App, isGameFocused) == 0x15A);
static_assert(offsetof(App, gameWidth) == 0x15C);
static_assert(offsetof(App, gameHeight) == 0x160);
static_assert(offsetof(App, graphics) == 0x174);
static_assert(offsetof(App, currentScene) == 0x178);
static_assert(offsetof(App, audioSystem) == 0x17C);
static_assert(offsetof(App, inputHandler) == 0x180);
static_assert(offsetof(App, gameUpdateObj) == 0x184);
static_assert(offsetof(App, rightButtonPauseEnabled) == 0x238);
static_assert(offsetof(App, musicHandle) == 0x534);
static_assert(offsetof(App, musicChannel1) == 0x538);
static_assert(offsetof(App, musicChannel2) == 0x53C);
static_assert(offsetof(App, gameMode1) == 0x550);
static_assert(offsetof(App, gameMode2) == 0x554);
static_assert(offsetof(App, gameMode3) == 0x558);
static_assert(offsetof(App, gameMode4) == 0x55C);
static_assert(offsetof(App, sensitivity) == 0x84C);
static_assert(offsetof(App, unlockMirrorTournament) == 0x850);
static_assert(offsetof(App, unlockDizzyRace) == 0x851);
static_assert(offsetof(App, unlockImpossibleArena) == 0x868);
static_assert(offsetof(App, bestTimes) == 0x86C);
static_assert(offsetof(App, medals) == 0x8BC);
static_assert(offsetof(App, p2Controller1) == 0xB28);
static_assert(offsetof(App, p2Controller2) == 0xB2C);
static_assert(offsetof(App, p2Controller3) == 0xB30);
static_assert(offsetof(App, p2Controller4) == 0xB34);

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
	virtual App* GetApp() = 0;
	virtual void UnlockAll() = 0;
	virtual void LockAll() = 0;
	virtual bool QuitGame() = 0; 
	virtual bool SaveConfig() = 0;
	virtual void ApplyForce(void* player, float x, float y, float z, float magnitude) = 0;
};

class HamsterballAPI {
public:
	virtual ~HamsterballAPI() {}
	virtual const char* GetModName() = 0;
	virtual void Initialize(IModAPI* loader) {}
	virtual void onPlayerUpdate(void* PlayerObject) {}
	virtual void onRenderApply(void* this_ptr, float* viewMatrix) {}
	virtual void onButtonToggle(const char* buttonId, bool newState) {}
	virtual void onGameUpdate() {}
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

// for __cdecl functions 
template <typename ReturnType = void, typename... Args>
ReturnType Call(DWORD offset, Args... args) {
	DWORD realAddress = (DWORD)GetModuleHandle(NULL) + offset;
	typedef ReturnType(__cdecl* GameFunc)(Args...);
	GameFunc func = (GameFunc)realAddress;
	return func(args...);
}

// for __thiscall methods
template <typename ReturnType = void, class ObjectType, typename... Args>
ReturnType CallMethod(DWORD offset, ObjectType* objPointer, Args... args) {
	DWORD realAddress = (DWORD)GetModuleHandle(NULL) + offset;
	typedef ReturnType(__thiscall* GameFunc)(ObjectType*, Args...);
	GameFunc func = (GameFunc)realAddress;
	return func(objPointer, args...);
}

// for __fastcall functions 
template <typename ReturnType = void, typename... Args>
ReturnType CallFast(DWORD offset, Args... args) {
	DWORD realAddress = (DWORD)GetModuleHandle(NULL) + offset;
	typedef ReturnType(__fastcall* GameFunc)(Args...);
	GameFunc func = (GameFunc)realAddress;
	return func(args...);
}