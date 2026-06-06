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
struct PhysicsObject;
struct App;
struct Ball;
struct Scene;

class IModAPI {
public:
	virtual ~IModAPI() {}

	
	virtual void RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) = 0;
	virtual bool IsKeyDown(int dik) = 0;
	virtual bool WasKeyPressed(int dik) = 0;
	virtual bool WasKeyReleased(int dik) = 0;
	virtual void CreateToggleButton(const char* id, const char* displayText, bool defaultState, Color color = Color()) = 0;
	
	virtual void PatchMemory(DWORD address, const char* bytes, size_t size) = 0;
	virtual void UnlockAll() = 0;
	virtual void LockAll() = 0;
	virtual bool QuitGame() = 0; 
	virtual bool SaveConfig() = 0;
	virtual void ApplyForce(Ball* player, float x, float y, float z, float magnitude) = 0;
	virtual void SetSpeed(Ball* player, float mult) = 0;

	virtual bool GetButtonState(const char* id) = 0;
	virtual Ball* GetPlayer() = 0;
	virtual Ball* GetPlayer2() = 0;
	virtual Ball* GetPlayer3() = 0;
	virtual Ball* GetPlayer4() = 0;

	// pass in &enemyCount to get the size of the array that is returned 
	virtual Ball** GetEnemies(size_t* enemyCount) = 0;

	virtual PhysicsObject* GetPhysicsObj() = 0;
	virtual Scene* GetScene() = 0;
	virtual DWORD GetGameBaseAddress() = 0;
	virtual App* GetApp() = 0;
};

class HamsterballAPI {
public:
	virtual ~HamsterballAPI() {}
	virtual const char* GetModName() = 0;
	virtual void Initialize(IModAPI* loader) {}
	virtual void onPlayerUpdate(Ball* PlayerObject) {}
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
	std::uint8_t pad_84[0x04];
	float cameraDamping; // 0x4CF3F0
	std::uint8_t pad_8C[0x4CF484 - 0x4CF3F4];
	float unknown5; // 0x4CF484
	std::uint8_t pad_120[4];
	float unknown6; // 0x4CF48C 
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Ball {
	// unverified means i don't know for sure that memory address is what i think it is
	void** vtable; // +0x000
	std::uint8_t pad_004[0x014 - 0x004];
	Scene* scene; // +0x014 unverified
	int playerID; // +0x018 
	std::uint8_t pad_01C[0x158 - 0x01C];
	float prev_pos_x; // +0x158
	float prev_pos_y; // +0x15C
	float prev_pos_z; // +0x160
	float pos_x; // +0x164
	float pos_y; // +0x168
	float pos_z; // +0x16C
	std::uint8_t pad_170[0x17C - 0x170];
	float accel_x; // +0x17C
	float accel_y; // +0x180
	float accel_z; // +0x184
	float max_speed; // +0x188
	std::uint8_t pad_18C[0x190 - 0x18C];
	float facing_angle; // +0x190
	std::uint8_t pad_194[0x1A0 - 0x194];
	float speed_mult; // +0x1A0
	PhysicsObject* physics_object; // +0x1A4 
	float gravity_vec[3]; // +0x1A8 can't change this, it always changes back to default 
	std::uint8_t pad_1B4[0x1C8 - 0x1B4];
	float ball_outline_opacity; // +0x1C8
	std::uint8_t pad_1CC[0x260 - 0x1CC];
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
	std::uint8_t pad_2CD[0x2D4 - 0x2CD];
	bool ball_shake; // +0x2D4 not sure if this is intended or what 
	std::uint8_t pad_2D5[0x2DC - 0x2D5];
	float checkpoint_x; // +0x2DC
	float checkpoint_y; // +0x2E0
	float checkpoint_z; // +0x2E4
	bool event_checkpoint_flag; // +0x2E8 unverified
	bool unknown; // +0x2E9 i don't know what this does, but it does break the ball if you set it to true
	std::uint8_t pad_2EA[0x310 - 0x2EA];
	bool state_active; // +0x310 unverified
	std::uint8_t pad_311[0x700 - 0x311];
	int sound_3d_handle; // +0x700 unverified
	std::uint8_t pad_704[0x748 - 0x704];
	int gravity_type; // +0x748 can be 0,1,2, causes crashes when outside of odd race
	std::uint8_t pad_74C[0x768 - 0x74C];
	bool cam_active; // +0x768
	std::uint8_t pad_769[0xC4C - 0x769];
	bool low_gravity_mode; // +0xC4C
	std::uint8_t pad_C4D[0xC50 - 0xC4D];
	float burn_amount; // +0xC50 how burnt the ball is (from the magnifying glass), 1 kills the player normally, but setting manually doesn't seem to do this
	std::uint8_t pad_C54[0xC88 - 0xC54];
	float world_matrix[16]; // +0xC88 unverified
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Scene {
	void** vtable; // +0x000
	std::uint8_t pad_004[0x014 - 0x004];
	App* owner_app; // +0x014
	std::uint8_t pad_018[0x868 - 0x018];
	char* name; // +0x868 
	std::uint8_t pad_86C[0x8AC - 0x86C];
	void* level_ptr; // +0x8AC unverified 
	void* skybox_ptr; // +0x8B0 unverfied
	std::uint8_t pad_8B4[0x29BC - 0x8B4];
	float camera_angle; // +0x29BC 
	float camera_distance; // +0x29C0
	std::uint8_t pad_29C4[0x29D0 - 0x29C4];
	Ball* current_ball_ptr; // +0x29D0 couldn't change 
	std::uint8_t pad_29D4[0x29D8 - 0x29D4];
	int ball_list_count; // +0x29D8
	std::uint8_t pad_29DC[0x2DE0 - 0x29DC];
	Ball** ball_list; // +0x2DE0
	std::uint8_t pad_2DE4[0x3620 - 0x2DE4];
	int frame_counter; // +0x3620 frames since start 
	std::uint8_t pad_3624[0x362C - 0x3624];
	void* player_list; // +0x362C unverified
	int player_count; // +0x3630 
	std::uint8_t pad_3634[0x3F1C - 0x3634];
	int path_follow_mode; // +0x3F1C unverified - i haven't been able to get this to work. but on 2p this is set to 0, and is another number one 1p
	void* cam_path_object; // +0x3F20 unverified
	float cam_path_position; // +0x3F24
	std::uint8_t pad_3F28[0x3F2C - 0x3F28];
	float cam_time_to_zoom; // +0x3F2C i didn't really know what to call this one, it's weird
	std::uint8_t pad_3F30[0x434C - 0x3F30];
	float cam_offset_x; // +0x434C
	float cam_offset_y; // +0x4350 
	float cam_offset_z; // +0x4354
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PhysicsObject {
	void** vtable; // +0x000
	std::uint8_t pad_004[0x010 - 0x004];
	Ball* owner_ball; // +0x10
	std::uint8_t pad_014[0x0C60 - 0x014];
	int unknown; // +0x0C60
	float speed_scalar; // +0x0C64 You can't manually change this 
	float friction; // +0x0C68
	std::uint8_t pad_0C6C[0x0C7C - 0x0C6C];
	bool noclip; // +0x0C7C requires no break mod otherwise
	std::uint8_t pad_0C7D[0x0C8C - 0x0C7D];
	float gravity_x; // +0x0C8C
	float gravity_y; // +0x0C90
	float gravity_z; // +0x0C94
	std::uint8_t pad_0C98[0x0CA4 - 0x0C98];
	float velocity_x; // +0x0CA4
	float velocity_y; // +0x0CA8
	float velocity_z; // +0x0CAC 
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
	std::uint8_t pad_178[0x17C-0x178];
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

static_assert(offsetof(Ball, vtable) == 0x000);
static_assert(offsetof(Ball, scene) == 0x014);
static_assert(offsetof(Ball, playerID) == 0x018);
static_assert(offsetof(Ball, prev_pos_x) == 0x158);
static_assert(offsetof(Ball, prev_pos_y) == 0x15C);
static_assert(offsetof(Ball, prev_pos_z) == 0x160);
static_assert(offsetof(Ball, pos_x) == 0x164);
static_assert(offsetof(Ball, pos_y) == 0x168);
static_assert(offsetof(Ball, pos_z) == 0x16C);
static_assert(offsetof(Ball, accel_x) == 0x17C);
static_assert(offsetof(Ball, accel_y) == 0x180);
static_assert(offsetof(Ball, accel_z) == 0x184);
static_assert(offsetof(Ball, max_speed) == 0x188);
static_assert(offsetof(Ball, speed_mult) == 0x1A0);
static_assert(offsetof(Ball, physics_object) == 0x1A4);
static_assert(offsetof(Ball, gravity_vec) == 0x1A8);
static_assert(offsetof(Ball, ball_outline_opacity) == 0x1C8);
static_assert(offsetof(Ball, boost_hit_flag) == 0x260);
static_assert(offsetof(Ball, rumble_timer1) == 0x264);
static_assert(offsetof(Ball, bounciness) == 0x278);
static_assert(offsetof(Ball, radius) == 0x284);
static_assert(offsetof(Ball, rumble_timer2) == 0x290);
static_assert(offsetof(Ball, spin_rate) == 0x2A4);
static_assert(offsetof(Ball, force_x) == 0x2BC);
static_assert(offsetof(Ball, force_y) == 0x2C0);
static_assert(offsetof(Ball, force_z) == 0x2C4);
static_assert(offsetof(Ball, disable_ball) == 0x2CC);
static_assert(offsetof(Ball, checkpoint_x) == 0x2DC);
static_assert(offsetof(Ball, checkpoint_y) == 0x2E0);
static_assert(offsetof(Ball, checkpoint_z) == 0x2E4);
static_assert(offsetof(Ball, event_checkpoint_flag) == 0x2E8);
static_assert(offsetof(Ball, unknown) == 0x2E9);
static_assert(offsetof(Ball, state_active) == 0x310);
static_assert(offsetof(Ball, sound_3d_handle) == 0x700);
static_assert(offsetof(Ball, cam_active) == 0x768);
static_assert(offsetof(Ball, low_gravity_mode) == 0xC4C);
static_assert(offsetof(Ball, world_matrix) == 0xC88);
static_assert(offsetof(Ball, facing_angle) == 0x190);
static_assert(offsetof(Ball, ball_shake) == 0x2D4);
static_assert(offsetof(Ball, gravity_type) == 0x748);
static_assert(offsetof(Ball, burn_amount) == 0xC50);

static_assert(offsetof(PhysicsObject, owner_ball) == 0x010);
static_assert(offsetof(PhysicsObject, unknown) == 0x0C60);
static_assert(offsetof(PhysicsObject, speed_scalar) == 0x0C64);
static_assert(offsetof(PhysicsObject, friction) == 0x0C68);
static_assert(offsetof(PhysicsObject, noclip) == 0x0C7C);
static_assert(offsetof(PhysicsObject, gravity_x) == 0x0C8C);
static_assert(offsetof(PhysicsObject, gravity_y) == 0x0C90);
static_assert(offsetof(PhysicsObject, gravity_z) == 0x0C94);
static_assert(offsetof(PhysicsObject, velocity_x) == 0x0CA4);
static_assert(offsetof(PhysicsObject, velocity_y) == 0x0CA8);
static_assert(offsetof(PhysicsObject, velocity_z) == 0x0CAC);
static_assert(sizeof(PhysicsObject) == 0x0CB0);

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

static_assert(offsetof(Scene, vtable) == 0x000);
static_assert(offsetof(Scene, owner_app) == 0x014);
static_assert(offsetof(Scene, name) == 0x868);
static_assert(offsetof(Scene, level_ptr) == 0x8AC);
static_assert(offsetof(Scene, skybox_ptr) == 0x8B0);
static_assert(offsetof(Scene, camera_angle) == 0x29BC);
static_assert(offsetof(Scene, camera_distance) == 0x29C0);
static_assert(offsetof(Scene, current_ball_ptr) == 0x29D0);
static_assert(offsetof(Scene, ball_list_count) == 0x29D8);
static_assert(offsetof(Scene, ball_list) == 0x2DE0);
static_assert(offsetof(Scene, frame_counter) == 0x3620);
static_assert(offsetof(Scene, player_list) == 0x362C);
static_assert(offsetof(Scene, player_count) == 0x3630);
static_assert(offsetof(Scene, cam_path_object) == 0x3F20);
static_assert(offsetof(Scene, cam_path_position) == 0x3F24);
static_assert(offsetof(Scene, cam_offset_x) == 0x434C);
static_assert(offsetof(Scene, cam_offset_y) == 0x4350);
static_assert(offsetof(Scene, cam_offset_z) == 0x4354);
static_assert(offsetof(Scene, path_follow_mode) == 0x3F1C);
static_assert(offsetof(Scene, cam_time_to_zoom) == 0x3F2C);

static_assert(offsetof(PhysicsConstants, unknown) == 0x00);
static_assert(offsetof(PhysicsConstants, dizzyForceMult) == 0x04);
static_assert(offsetof(PhysicsConstants, glassForceMult) == 0x0C);
static_assert(offsetof(PhysicsConstants, unknown2) == 0x10);
static_assert(offsetof(PhysicsConstants, unknown3) == 0x18);
static_assert(offsetof(PhysicsConstants, hamsterSize) == 0x34);
static_assert(offsetof(PhysicsConstants, unknown4) == 0x80);
static_assert(offsetof(PhysicsConstants, cameraDamping) == 0x88);
static_assert(offsetof(PhysicsConstants, unknown5) == 0x11C);
static_assert(offsetof(PhysicsConstants, unknown6) == 0x124);