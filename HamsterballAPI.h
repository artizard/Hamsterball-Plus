#pragma once
#include <windows.h>
#include <math.h>
#include <cstdint>
#include <cstddef>
#include <dinput.h>
#include <cstdio>
#define DIRECTINPUT_VERSION 0x0800

#define HAMSTERBALL_API_VERSION 1

struct Collision;
class HamsterballAPI;

#pragma pack(push, 1)
/// A general color struct to be used with some of the functions. The default constructor sets rgba all to 1.0f.
struct Color {
	float r, g, b, a;
	// default (white)
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	// custom color select constructor
	Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
};

/// @brief The struct used for creating custom buttons for the option menu (specifically the yes/no toggleable buttons)
struct CustomButton {
	const char* id; // The internal ID for the button. Use a unique name that other mods are unlikely to use
	const char* displayText; // The name for the button that will be seen in game
	bool defaultState = false; // The default state when the user uses the mod for the first time (ex. default to off)
	const char* trueText = "YES"; // The text that will be displayed for the on/true state 
	const char* falseText = "NO"; // The text that will be displayed for the off/false state 
	Color color = Color(); // The color of the button text (Will be white if you don't change this)

	CustomButton() = default;

	CustomButton(const char* id, const char* displayText) : id(id), displayText(displayText) {}
};

/// @brief The struct used for creating custom sliders in the option menu (these are controlled with the arrow keys). These are
/// not literal sliders like in the vanilla game's menu, but these show a number instead (due to technical limitations). 
struct CustomSlider {
	const char* id; // The internal ID for the slider. Use a unique name that other mods are unlikely to use
	const char* displayText; // The name for the slider that will be seen in game
	float startingValue; // The default value for when the user uses the mod for the first time
	float stepSize = .1; // The increments in which the user can change the slider 
	int decimalPlaces = 2; // The amount of decimal places you want the slider to show 
	float lowerBound = -INFINITY; // The lowest that the slider can go (defaults to no lower bound) 
	float upperBound = INFINITY; // The highest that the slider can go (defaults to no upper bound)
	const char* unitName = ""; // The unit shown after the number. Leave default for no unit. 
	Color color; // The color of the slider text

	CustomSlider() = default;

	CustomSlider(const char* id, const char* displayText, float startingValue) :
		id(id), displayText(displayText), startingValue(startingValue) {
	}
};

/// @brief A struct used when calling the text drawing functions. This is just how you input the parameters. 
struct CustomText {
	void* font; // use a value from the Fonts struct
	int x = 0; // x position of text
	int y = 0; // y position of text
	bool enable_shadow = true; // whether or not you want a text shadow, if false, then just leave the shadow related fields default 
	int shadow_x = 5; // x offset for the text shadow
	int shadow_y = 5; // y offset for the text shadow
	Color text_color = Color(1, 1, 1, 1);
	Color shadow_color = Color(0, 0, 0, 1); // defaults to black 

	CustomText() = default;

	CustomText(void* font, int x, int y, Color text_color, bool enable_shadow) :
		font(font), x(x), y(y), text_color(text_color), enable_shadow(enable_shadow) {
	}
};

/// @brief Struct used when registering a custom control. 
struct CustomControl {
	int dikCode;
	bool requiresCtrl = false;

	CustomControl() = default;
	CustomControl(int dikCode) : dikCode(dikCode) {}
	CustomControl(int dikCode, bool requiresCtrl) : dikCode(dikCode), requiresCtrl(requiresCtrl) {}
};
#pragma pack(pop)

// forward declarations
struct PhysicsObject;
struct App;
struct Ball;
struct Scene;
struct Vec3;
struct PhysicsConstants;
struct Sounds;
struct Fonts;

/// This class is how you will retrieve values and call functions on the game. 
class IModAPI {
public:
	/// @brief Deconstructor for IModAPI. Only worry about this if you need to free up memory when the game closes.
	virtual ~IModAPI() {}

	/// @brief Create a custom hook. You should use the preexisting functions like onGameUpdate when possible, but if the 
	/// hook is not already created, then you'll have to do it using this. This can cause issues if another mod also hooks 
	/// into the same function. This is one of the more complicated aspects of this modding API, so there will be examples
	/// for using this. (There is some boilerplate code you will have to use in conjunction with this function)
	/// @param targetAddress Address of the function to hook into. 
	/// @param hookFunction The function that the original function will redirect to.
	/// @param original The original game's function, you can call this to get back the original functionality.
	virtual void RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) = 0;

	/// @brief Create an id for a custom control that the user can remap on their own. Using this as opposed to hardcoding the
	/// keycodes allows the user to remap in case the default value you give conflicts with another mod. 
	/// @param controlID The id for the control; use a unique name that other mods are unlikely to use, but keep it clear as to what it does.
	/// @param default_dik The default DirectInput keycode that maps to your control. The user can rebind this, this will just be the default. 
	virtual void RegisterCustomControl(const char* controlID, CustomControl defaultControl) = 0;

	/// @brief Returns a CustomControl struct which contains the DirectInput 8 DIK code that corresponds to the control id given as well as whether or not
	/// the control also requires ctrl to be pressed down (for keybinds like ctrl+r). If the user didn't customize the control then
	/// this will just be the default value you chose. 
	/// @param controlID The id for the control you want the keycode for
	/// @return The DirectInput keycode for the control, -1 if the control was not found
	virtual CustomControl GetCustomControlKey(const char* controlID) = 0;

	/// @brief USE IsControlDown INSTEAD IN MOST CASES - Checks if a key is currently being pressed down. 
	/// Use this within onGameUpdate() or onBallUpdate()
	/// @param dik DirectInput keycode to check
	/// @return Whether the key was pressed down or not
	virtual bool IsKeyDown(int dik) = 0;

	/// @brief USE WasControlPressed INSTEAD IN MOST CASES - Checks if a key was pressed. This will trigger once per press;
	/// if the user holds a key down, then this will only return true once. Use this within onGameUpdate() or onBallUpdate()
	/// @param dik DirectInput keycode to check
	/// @return Whether the key was pressed or not
	virtual bool WasKeyPressed(int dik) = 0;

	/// @brief USE WasControlReleased IN MOST CASES - Checks if a key was released. This is in the case that the key was
	/// being held down before, but has now been released. Use this within onGameUpdate() or onBallUpdate()
	/// @param dik DirectInput keycode to check
	/// @return Whether the key was release or not
	virtual bool WasKeyReleased(int dik) = 0;

	/// @brief Checks if a control is currently being pressed down. Use this within onGameUpdate() or onBallUpdate()
	/// @param controlID The control id to check
	/// @return Whether the control was pressed down or not. Defaults to false if the control ID is invalid
	virtual bool IsControlDown(const char* controlID) = 0;

	/// @brief Checks if a control was pressed. This will trigger once per press; if the user holds a control down, then this 
	/// will only return true once. Use this within onGameUpdate() or onBallUpdate()
	/// @param controlID The control id to check
	/// @return Whether the control was pressed or not. Defaults to false if the control ID is invalid
	virtual bool WasControlPressed(const char* controlID) = 0;

	/// @brief Checks if a control was released. This is in the case that the control was being held down before, but has 
	/// now been released. Use this within onGameUpdate() or onBallUpdate()
	/// @param controlID The control id to check
	/// @return Whether the control was released or not. Defaults to false if the control ID is invalid
	virtual bool WasControlReleased(const char* controlID) = 0;

	/// @brief Create a toggleable option in the game's option menu. 
	/// @param button The button struct that defines all of the parameters of the button. Read those comments for more information.
	/// @param this Just pass in 'this' as the parameter
	virtual void CreateToggleButton(const CustomButton& button, HamsterballAPI* modInstance) = 0;

	/// @brief Create a slider in the game's option menu.
	/// @param slider The slider struct that defines all of the parameters of the slider. Read those comments for more information.
	/// @param this Just pass in 'this' as the parameter
	virtual void CreateSlider(const CustomSlider& slider, HamsterballAPI* modInstance) = 0;

	/// @brief Patches memory within Hamsterball.exe. This is temporary, as it does not alter the actual .exe, it just modifies the 
	/// current instance of the game in memory. I'd recommend using this within Initialize() or onButtonToggle(). 
	/// @param address The address to patch from
	/// @param bytes A char* of the new bytes to patch. An example: "\xC6\x85\xE9\x02\x00\x00\x01"
	/// @param size How many bytes you want to patch
	virtual void PatchMemory(DWORD address, const char* bytes, size_t size) = 0;

	/// @brief Unlocks all levels and arenas in the game (plus mirror tournament)
	virtual void UnlockAll() = 0;

	/// @brief Locks all levels and arenas in the game (plus mirror tournament)
	virtual void LockAll() = 0;

	/// @brief Quits the game gracefully without a crash message. 
	/// @return Returns whether it successfully closed. (It realistically should work in all cases)
	virtual bool QuitGame() = 0;

	/// @brief Saves the current configs to the registry such as resolution, unlocks, etc. Use this to save new changes to the 
	/// options that would be in the registry. These will automatically be saved when the game closes (provided it doesn't crash),
	/// but it's safer to call this right after. 
	/// @return Whether or not the config was saved. (Should realistically always work)
	virtual bool SaveConfig() = 0;

	/// @brief Applies a force to the given ball object. I'm not sure if x,y,z need to add up to 1, so keep that in mind if issues arise.
	/// @param ball The ball that you want to apply the force to. Use it on api->GetPlayer() to do it to player 1. 
	/// @param x 0.0-1.0 value for the x direction of the force
	/// @param y 0.0-1.0 value for the y direction of the force
	/// @param z 0.0-1.0 value for the z direction of the force
	/// @param magnitude Strength of the force 
	virtual void ApplyForce(Ball* ball, float x, float y, float z, float magnitude) = 0;

	/// @brief Sets the given ball's speed (magnitude of its velocity). 
	/// @param ball The ball you want to set the speed of 
	/// @param mult The speed multiplier; 1.0 would do nothing, 0.5 would halve speed, 2.0 would double speed, etc. 
	virtual void SetSpeed(Ball* ball, float mult) = 0;

	/// @brief Gets the state of a option toggle button. 
	/// @param id ID of button
	/// @return The bool value of the button's current state. Returns false if the button ID was invalid. 
	virtual bool GetButtonState(const char* id) = 0;

	/// @brief Gets the current value of a custom slider on the option menu. 
	/// @param id ID of the slider
	/// @return The value of the chosen slider. Returns -1 for invalid slider IDs. 
	virtual float GetSliderState(const char* id) = 0;

	/// @brief Gets player 1. Will be a nullptr if the player doesn't exist (not in level)
	/// @return The Ball* to player 1
	virtual Ball* GetPlayer() = 0;

	/// @brief Gets player 2. Will be a nullptr if the player doesn't exist (not in level or wrong number of players)
	/// @return The Ball* to player 2
	virtual Ball* GetPlayer2() = 0;

	/// @brief Gets player 3. Will be a nullptr if the player doesn't exist (not in level or wrong number of players)
	/// @return The Ball* to player 3
	virtual Ball* GetPlayer3() = 0;

	/// @brief Gets player 4. Will be a nullptr if the player doesn't exist (not in level or wrong number of players)
	/// @return The Ball* to player 4
	virtual Ball* GetPlayer4() = 0;

	/// @brief Get a list of the 8balls currently active. 
	/// @param enemyCount Pass in a pointer, this is the number of enemies that will be in the returned list. 
	/// @return Ball** list of the 8balls 
	virtual Ball** GetEnemies(size_t* enemyCount) = 0;

	/// @brief Shorthand to get the PhysicsObject of player 1. For other players/8balls, you can access them through the
	/// Ball struct. This will be nullptr if there is no player 1 (such as in the menu). 
	/// @return Player 1's PhysicsObject
	virtual PhysicsObject* GetPhysicsObj() = 0;

	/// @brief Get the current active scene (the level). If there is no active scene, then this will return a nullptr, 
	/// so make sure to do error handling in your mod. 
	/// @return The current scene object
	virtual Scene* GetScene() = 0;

	/// @brief Gets the base address of the game. Use this to get full addresses of function and such, such as when registering a hook. 
	/// @return The game's base address 
	virtual DWORD GetGameBaseAddress() = 0;

	/// @brief Get the game's App object. There is only ever one of these created (to my knowledge)
	/// @return The game's App object
	virtual App* GetApp() = 0;

	/// @brief WARNING Allocates memory similar to malloc but in such a way that the game can free. Be very careful with this, I do not know exactly 
	/// how the game frees memory. I personally just used this when I reverse engineered spawning 8balls. This is useful for replacing
	/// operator_new calls in the decompiled code - but make sure the memory actually gets freed if you use this. 
	/// @param size Number of bytes to allocate
	/// @return Pointer to the chunk of memory you allocated 
	virtual void* AllocateMem(unsigned int size) = 0;

	/// @brief Creates an 8ball (badball) at the given position. The only required parameters are spawn_pos and home_pos, the others can be
	/// left off, in which case the default game values will be used. If you have too many 8balls active at once, the game will crash  
	/// (but it can handle a fair amount) 
	/// @param spawn_pos Position where the ball should spawn.
	/// @param home_pos Where the ball will go back to when idle. 
	/// @param home_distance How far the ball is allowed to go from its home. 
	/// @param chase_distance How far it can see the player from. 
	/// @param radius How large the ball is. 
	/// @param spin_distance How big the circles it makes while idle are. 
	virtual void CreateBadBall(Vec3 spawn_pos, Vec3 home_pos, float home_distance = 200, float chase_distance = 300, float radius = 35, float spin_distance = 45) = 0;

	/// @brief Reloads the ModConfig.ini file. This means it will update controls, colors, etc. This will not load in the values stored for the 
	/// toggle/slider options, those are applied at launch. 
	virtual void ReloadIniFile() = 0;

	/// Returns the current time on the game's timer (the value when called, not a pointer). This number counts up in time trials
	/// and down in tournament. This also counts up for the arenas, but it isn't really used for anything in the actual game. 
	/// @return The current timer time
	virtual int GetTimerTime() = 0;

	/// Sets the time on the in game timer. In time trials this is the amount of time passed, while in tournament this is the amount of
	/// time left.
	/// @param time The time you want to set the timer to. Note that this "1000" is 10 seconds, and "575" is 5.75 seconds. 
	virtual void SetTimerTime(int time) = 0;

	/// @brief Sends out a ray (actually a spehere) and returns the vector of where the ray hit. If it doesn't hit anything, then the distance 
	/// will be roughly 994.45f. I would generally recommend to use LevelRaycastHit instead, this returns just the vector, so you have to process
	/// it yourself. THIS ONLY ACCOUNTS FOR LEVEL GEOMETRY AND SOME ENEMIES, THE RAYS WILL IGNORE BADBALLS / OTHER PLAYERS.
	/// @param position Where the ray should be cast from
	/// @param direction What direction the ray should go in (ex. (0,-1,0))
	/// @param radius The size of the sphere; generally small values are better, but larger values can represent the player better as the ray can't fit through small gaps
	/// @return Vector of where the ray hits
	virtual Vec3 LevelRaycastVec(Vec3 position, Vec3 direction, float radius) = 0;

	/// @brief Sends out a ray (actually a spehere) and returns whether or not the ray hit anything within a given distance. 
	/// THIS ONLY ACCOUNTS FOR LEVEL GEOMETRY, THE RAYS WILL IGNORE BADBALLS, PLAYERS, ETC.
	/// @param position Where the ray should be cast from
	/// @param direction What direction the ray should go in (ex. (0,-1,0))
	/// @param radius The size of the sphere; generally small values are better, but larger values can represent the player better as the ray can't fit through small gaps
	/// @param max_dist The max distance you want the ray to travel. Leave default/-1 to leave the distance uncapped. 
	/// @return Vector of where the ray hits
	virtual bool LevelRaycastHit(Vec3 position, Vec3 direction, float radius, float max_dist = -1) = 0;

	/// @brief Gives a pointer to the physics constants struct. You can change different values which affect the game globally such as glass friction and hamster size.
	/// @return Pointer the the game's physics constants
	virtual PhysicsConstants* GetPhysicsConstants() = 0;

	/// @brief Plays the given sound effect. You can find the sound effects to use from the sound struct within the App struct. 
	/// @param soundEffect The sound to play
	/// @param volume The volume at which to play the sound. The range is from 0-1.
	virtual void PlaySoundEffect(void* soundEffect, float volume) = 0;

	/// @brief Plays a given sound effect at a certain position in 3d space. This means that the volume will be dependent on the distance from the player.
	/// Don't expect directional sound or anything, this is the same as PlaySoundEffect except it scales the volume based on distance from the player. 
	/// @param soundEffect The sound to play
	/// @param position Where to play the sound from
	/// @param volume The volume at which to play the sound. The range is from 0-1.
	virtual void Play3dSoundEffect(void* soundEffect, Vec3 position, float volume) = 0;

	/// @brief Displays a message above the ball, like when unlocking an arena. THIS ONLY WORKS IN TOURNAMENT MODE
	/// @param ball The ball to place the message over
	/// @param message The message to display
	virtual void ShowBallMessage(Ball* ball, char* message) = 0;

	/// @brief Respawns the provided player when called.
	/// @param The player you want to respawn. This technically works on badballs, but breaks their AIs and collision.
	virtual void RespawnPlayer(Ball* player) = 0;

	/// @brief Draws text onto the screen. This only applies to the current frame, so this needs to be used in onTextRenderLoop. Use this for more complex logic such
	/// as moving/changing text. For simple messages, just use DrawTimedMessage(). 
	/// @param text The text you want to display
	/// @param params Options for the text: font, color, etc.
	virtual void DrawCustomText(const char* text, const CustomText& params) = 0;

	/// @brief Draws text onto the screen for a specific amount of time. You can call this from wherever you want unlike DrawCustomText(). 
	/// @param text The text you want to display
	/// @param params Options for the text: font, color, etc.
	/// @param messageDuration How long you want the message to last on screen, in seconds. 
	virtual void DrawTimedMessage(const char* text, const CustomText& params, float messageDuration) = 0;

	/// @brief Returns the speed of a ball. This is just the magnitude of the velocity. 
	/// @param ball The ball you want the speed of
	/// @return The speed of the ball
	virtual float GetBallSpeed(Ball* ball) = 0;

	/// @brief Shatters (breaks) the given ball. This will not work if the nobreak mod is on. 
	/// @param ball The ball you want to shatter
	virtual void ShatterBall(Ball* ball) = 0;
};

/// This includes functions that you can override in order to add logic on certain events such as onBallUpdate,
/// onButtonToggle, onGameUpdate(), etc. 
class HamsterballAPI {
public:
	/// @brief Deconstructor for HamsterballAPI. Only worry about this if you need to free up memory when the game closes.
	virtual ~HamsterballAPI() {}

	/// @brief This is a required function to implement. This should return the name of the mod. 
	/// @return Name of the mod
	virtual const char* GetModName() = 0;

	/// @brief This is a required function to implement. This should return the name of the author of the mod. 
	/// @return Author of the mod
	virtual const char* GetAuthorName() = 0;

	/// @brief Requred function that returns the version of the modding API used in the mod. Use the boilerplate code shown in the tutorial/other mods. 
	/// @return The version of the mod. 
	virtual int GetApiVersion() = 0;

	/// @brief Use this if anyone else helped with the mod. Return their names like this "contributor1, contributor2"
	/// @return The names of the contributors
	virtual const char* GetContributors() { return ""; }

	/// Creates instancce of IModAPI. This function isn't necessarily required, but you'll need it if you want to use anything from
	/// IModAPI. Additionally you can put code in here that you want to run when the mod launches. (Setting up members, etc.)
	virtual void Initialize(IModAPI* loader) {}

	/// @brief Put logic here that you want to run every ball (player or badballs) update. This is good for things like handling controls and
	/// other player related stuff that needs to be done every tick. This does not run while you are in the main menu. Keep in mind this runs for each ball. 
	/// This corresponds to the function at 0x405E00
	/// @param ball The player that is being updated
	virtual void onBallUpdate(Ball* ball) {}

	/// @brief Hook for the onRenderApply function (0x454830). 
	/// @param this_ptr Not exactly sure what this is (came from the original function) 
	/// @param viewMatrix The camera's viewMatrix
	virtual void onRenderApply(void* this_ptr, float* viewMatrix) {}

	/// @brief Put logic here that you want to run when a toggle button option is enabled or disabled. From there, you can use if statements to see
	/// if the buttonId matches one of your custom ones, and then carry out logic from there. The hooked function is 0x4434F0
	/// @param buttonId The ID of the button that was clicked
	/// @param newState The new state of that button
	virtual void onButtonToggle(const char* buttonId, bool newState) {}

	virtual void onSliderChange(const char* sliderId, float newValue) {}

	/// @brief Put logic here that you want to run every tick. This is good for controls that you want to work whenever,
	/// not just in levels like with onBallUpdate(). The corresponding hooked function is 0x46C170
	virtual void onGameUpdate() {}

	/// @brief You can put logic for event plane collisions here. This allows you to put logic for different types of event planes.
	/// For instance, you can add logic for when the player hits the goal. Another massive possibility with this is adding custom event
	/// planes. If you make a custom one in blender then export as a custom level, you can add an if statement to see if eventPlaneID matches
	/// the custom plane you made. This has a lot of possibilities such as making the camera move, spawning an 8ball, etc. 
	/// Corresponding hooked function is 0x40C5D0.
	/// @param colliding_ball The ball that collided with the event plane
	/// @param eventPlaneID The ID of event plane that was hit ("N:GOAL", "E:LIMIT", etc.)
	virtual void onEventPlaneCollide(Ball* colliding_ball, char* eventPlaneID) {}

	/// @brief Use DrawCustomText() within this loop. This is just a place where you put the text logic that should run every frame. 
	virtual void onTextRenderLoop() {}

	/// @brief Runs when two balls collide with each other. Keep in mind that the collisions are asymmetric; if the player and a badball collides, you will either get 
	/// a call to this like (player, badball) or (badball, player). You will not get it both ways, so you will have to check both ways. 
	/// @param ball1 The first ball involved in the collision
	/// @param ball2 The first ball involved in the collision
	virtual void onBallBump(Ball* ball1, Ball* ball2) {}

	/// @brief Runs when a scene ends, which is when a level ends (when the level entirely ends such as leaving the level or after the results menu). 
	/// (this is a hook of the Scene deconstructor) nThis could be good for freeing up memory at the end of a level, or just general logic when the level
	/// ends. Keep in mind this will also run if the player leaves the level early. If you want logic only for if the player finishes the level, you can use
	/// onEventPlaneCollide() to check if N:GOAL was hit. 
	virtual void onSceneEnd() {}

	/// @brief This runs logic when the level starts. This is when the level is loading in and the objects are initiated, before the countdown starts. 
	virtual void onLevelStart() {}
};

typedef HamsterballAPI* (*CreateModFunct)();


/// Simple 3D Vector struct
struct Vec3 {
	float x, y, z;
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

/// Vector subtraction helper function
/// @param a first vector
/// @param b second vector
/// @return difference of the two vectors
inline Vec3 Subtract(Vec3 a, Vec3 b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }

/// Vector cross product helper function
/// @param a first vector
/// @param b second vector
/// @return cross product of the two vectors
inline Vec3 Cross(Vec3 a, Vec3 b) {
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

/// Vector dot product helper function
/// @param a first vector
/// @param b second vector
/// @return dot product of the two vectors
inline float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

/// Helper function to normalize a vector
/// @param v the vector to be normalized
/// @return Normalized version of the vector
inline Vec3 Normalize(Vec3 v) {
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0.0f) return Vec3(0, 0, 0);
	return Vec3(v.x / length, v.y / length, v.z / length);
}

/// Builds a DirectX 8 compatible View Matrix (16 floats)
/// @param outMatrix A pointer to the game's matrix
/// @param eye The position of the camera 
/// @param target The position in the world that the camera is looking at 
/// @param up The vector defining what is "up"
inline void BuildCustomViewMatrix(float* outMatrix, Vec3 eye, Vec3 target, Vec3 up) {
	Vec3 zaxis = Normalize(Subtract(target, eye));
	Vec3 xaxis = Normalize(Cross(up, zaxis));
	Vec3 yaxis = Cross(zaxis, xaxis);

	outMatrix[0] = xaxis.x;           outMatrix[1] = yaxis.x;           outMatrix[2] = zaxis.x;           outMatrix[3] = 0.0f;
	outMatrix[4] = xaxis.y;           outMatrix[5] = yaxis.y;           outMatrix[6] = zaxis.y;           outMatrix[7] = 0.0f;
	outMatrix[8] = xaxis.z;           outMatrix[9] = yaxis.z;           outMatrix[10] = zaxis.z;          outMatrix[11] = 0.0f;
	outMatrix[12] = -Dot(xaxis, eye); outMatrix[13] = -Dot(yaxis, eye); outMatrix[14] = -Dot(zaxis, eye); outMatrix[15] = 1.0f;
}

// I'm not confident that it matters too much if you use the right one here, but I generally just try to match the game's function declaration
// to the best of my ability. 

/// @brief A generic function to call game functions that use "__cdecl" by their memory addresses. 
/// @tparam ReturnType The data type of the value returned by the game's function.
/// @tparam ...Args The data types of the arguments called with the game's function. 
/// @param offset The offset (relative to the game's exe) where the function is located
/// @param ...args The function's arguments
/// @return Whatever was returned by the function you call. 
template <typename ReturnType = void, typename... Args>
ReturnType Call(DWORD offset, Args... args) {
	DWORD realAddress = (DWORD)GetModuleHandle(NULL) + offset;
	typedef ReturnType(__cdecl* GameFunc)(Args...);
	GameFunc func = (GameFunc)realAddress;
	return func(args...);
}
/// @brief A generic function to call game methods that use "__thiscall" by their memory addresses. 
/// @tparam ReturnType The data type of the value returned by the game's method.
/// @tparam ...Args The data types of the arguments called with the game's method. 
/// @param offset The offset (relative to the game's exe) where the method is located
/// @param ...args The method's arguments
/// @return Whatever was returned by the method you call. 
template <typename ReturnType = void, class ObjectType, typename... Args>
ReturnType CallMethod(DWORD offset, ObjectType* objPointer, Args... args) {
	DWORD realAddress = (DWORD)GetModuleHandle(NULL) + offset;
	typedef ReturnType(__thiscall* GameFunc)(ObjectType*, Args...);
	GameFunc func = (GameFunc)realAddress;
	return func(objPointer, args...);
}
/// @brief A generic function to call game functions that use "__fastcall" by their memory addresses. 
/// @tparam ReturnType The data type of the value returned by the game's function.
/// @tparam ...Args The data types of the arguments called with the game's function. 
/// @param offset The offset (relative to the game's exe) where the function is located
/// @param ...args The function's arguments
/// @return Whatever was returned by the function you call. 
template <typename ReturnType = void, typename... Args>
ReturnType CallFast(DWORD offset, Args... args) {
	DWORD realAddress = (DWORD)GetModuleHandle(NULL) + offset;
	typedef ReturnType(__fastcall* GameFunc)(Args...);
	GameFunc func = (GameFunc)realAddress;
	return func(args...);
}

#pragma pack(push, 1)
/// @brief A struct representing some different physics constants that the game uses. Note that many of these
/// are unknown, so if you figure out what they do, please let me know. 
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
/// @brief A struct representing the game's Ball object. The Ball object represents players, as well as 8balls (badballs).
/// Fields labeled unverifed mean that the field may be wrong.
/// Keep in mind that not only is the struct likely missing some fields, but some of them cannot be edited in the way 
/// that you may hope. Some can only be read, while others will allow you to change the value. 
struct Ball {
	// unverified means i don't know for sure that memory address is what i think it is
	void** vtable; // +0x000
	std::uint8_t pad_004[0x014 - 0x004];
	Scene* scene; // +0x014 unverified
	int playerID; // +0x018 -1: badball, 0: player 1, 1: player 2, 2: player3, 3:player4
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
	float gravity_vec[3]; // +0x1A8 can't change this, it always changes back to default. I've seen weird cases where this is between 0 and 1, I would recommend just using the gravity params within PhysicsObject. 
	std::uint8_t pad_1B4[0x1C8 - 0x1B4];
	float ball_outline_opacity; // +0x1C8
	std::uint8_t pad_1CC[0x260 - 0x1CC];
	bool boost_hit_flag; // +0x260 unverified 
	std::uint8_t pad_261[0x264 - 0x261];
	std::uint8_t rumble_timer1[0x14]; // +0x264 unverified
	float bounciness; // +0x278
	std::uint8_t pad_27C[0x284 - 0x27C];
	float radius; // +0x284 the player is 26 by default
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
	bool low_gravity_mode; // +0xC4C This is not exactly what it seems, I would recommend altering gravity_y instead of using this. 
	std::uint8_t pad_C4D[0xC50 - 0xC4D];
	float burn_amount; // +0xC50 how burnt the ball is (from the magnifying glass), 1 kills the player normally, but setting manually doesn't seem to do this
	std::uint8_t pad_C54[0xC60 - 0xC54];
	float home_position_x; // +0xC60 only valid for badball, where the ball stays when not in chase
	float home_position_y; // +0xC64
	float home_position_z; // +0xC68
	float home_distance; // +0xC6C only valid for badball, how far badball will go from home_position
	float chase_distance; // +0xC70 only valid for badball, range in which badball can see players 
	bool is_badball_on_screen; // +0xC74 
	std::uint8_t pad_C75[0xC78 - 0xC75];
	float spin_counter; // +0xC78 only valid for badball, goes up for while the 8ball is spinning
	float spin_distance; // +0xC7C only valid for badball, how far 8ball spins (idle animation) 
	std::uint8_t pad_C80[0xC88 - 0xC80];
	float world_matrix[16]; // +0xC88 unverified
};
#pragma pack(pop)

#pragma pack(push, 1)
/// @brief A struct representing the game's Scene object. A Scene is the current level in play. 
/// Fields labeled unverifed mean that the field may be wrong.
/// Keep in mind that not only is the struct likely missing some fields, but some of them cannot be edited in the way 
/// that you may hope. Some can only be read, while others will allow you to change the value. 
struct Scene {
	void** vtable; // +0x000
	std::uint8_t pad_004[0x014 - 0x004];
	App* owner_app; // +0x014
	std::uint8_t pad_018[0x868 - 0x018];
	char* name; // +0x868 
	std::uint8_t pad_86C[0x8AC - 0x86C];
	void* level_ptr; // +0x8AC unverified 
	Collision* collision_mesh; // +0x8B0 
	std::uint8_t pad_8B4[0x29BC - 0x8B4];
	float camera_angle; // +0x29BC 
	float camera_distance; // +0x29C0
	std::uint8_t pad_29C4[0x29D0 - 0x29C4];
	Ball* current_ball_ptr; // +0x29D0 couldn't change
	void* ball_list; // +0x29D4 dynamic list struct
	int ball_list_count; // +0x29D8
	std::uint8_t pad_29DC[0x2DE0 - 0x29DC];
	Ball** ball_array; // +0x2DE0 actual array pointer 
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
	std::uint8_t pad_4358[0x47AC - 0x4358];
	int arena_timer; // +0x47AC
	bool timer_started; // +0x47B0
	std::uint8_t pad_47B1[0x47B4 - 0x47B1];
	int p1Score; // +0x47B4
	int p2Score; // +0x47B8
	int p3Score; // +0x47BC
	int p4Score; // +0x47C0
	bool weird_camera; // +0x47C4
	bool is_tiebreaker; // +0x47C5
};
#pragma pack(pop)

#pragma pack(push, 1)
/// @brief A struct representing the game's PhysicsObject. This is a sub-struct of the Ball struct. 
/// Fields labeled unverifed mean that the field may be wrong.
/// Keep in mind that not only is the struct likely missing some fields, but some of them cannot be edited in the way 
/// that you may hope. Some can only be read, while others will allow you to change the value. 
struct PhysicsObject {
	void** vtable; // +0x000
	std::uint8_t pad_004[0x010 - 0x004];
	Ball* owner_ball; // +0x10
	std::uint8_t pad_014[0x01C - 0x014];
	int collision_count; // +0x1C
	std::uint8_t pad020[0x424 - 0x020];
	void* collision_arr; // +0x424
	std::uint8_t pad428[0xC60 - 0x428];
	int unknown; // +0x0C60
	float speed_scalar; // +0x0C64 You can't manually change this 
	float friction; // +0x0C68
	std::uint8_t pad_0C6C[0x0C7C - 0x0C6C];
	bool noclip; // +0x0C7C requires no break mod otherwise the ball will break.
	std::uint8_t pad_0C7D[0x0C8C - 0x0C7D];
	float gravity_x; // +0x0C8C I would use this as opposed to the vector in Ball
	float gravity_y; // +0x0C90 I would use this as opposed to the vector in Ball
	float gravity_z; // +0x0C94 I would use this as opposed to the vector in Ball
	std::uint8_t pad_0C98[0x0CA4 - 0x0C98];
	float velocity_x; // +0x0CA4
	float velocity_y; // +0x0CA8
	float velocity_z; // +0x0CAC 
};
#pragma pack(pop)

#pragma pack(push, 1)
/// @brief These are pointers to the sounds you can use in PlaySoundEffect() and Play3dSoundEffect(). 
struct Sounds {
	void* collide;          // +0x000  (App+0x43C)
	void* roll;             // +0x004
	void* whistle;          // +0x008
	void* bumper;           // +0x00C
	void* ballbreak;        // +0x010
	void* ballbreaksmall;   // +0x014
	void* thwomp;           // +0x018
	void* snap;             // +0x01C
	void* popup;            // +0x020
	void* dropin;           // +0x024
	void* dropinshort;      // +0x028
	void* popout;           // +0x02C
	void* pipebump1;		// +0x030  
	void* pipebump2;		// +0x034
	void* pipebump3;		// +0x038
	void* gearclank;        // +0x03C
	void* bridgeslam;       // +0x040
	void* platformtick;     // +0x044
	void* gluestuck;        // +0x048
	void* bubble1;          // +0x04C
	void* bubble2;          // +0x050
	void* wheelcreak;       // +0x054
	void* catapult;         // +0x058
	void* trapdoor;         // +0x05C
	void* fwing;            // +0x060
	void* clink;            // +0x064
	void* whoosh;           // +0x068
	void* chomp;            // +0x06C
	void* fan_start;        // +0x070
	void* fan_blow;         // +0x074
	void* crack;            // +0x078
	void* crumble;          // +0x07C
	void* sawstartup;       // +0x080
	void* sawcut;           // +0x084
	void* minipop;          // +0x088
	void* bell;             // +0x08C
	void* zip;              // +0x090
	void* ting;             // +0x094
	void* shrink;           // +0x098
	void* grow;             // +0x09C
	void* tweet;            // +0x0A0
	void* creakyplatform;   // +0x0A4
	void* wubba;            // +0x0A8
	void* saw;              // +0x0AC
	void* sawspeedy;        // +0x0B0
	void* dawgstep1;        // +0x0B4
	void* dawgstep2;        // +0x0B8
	void* dawgsmash;        // +0x0BC
	void* sizzle;           // +0x0C0
	void* explode;          // +0x0C4
	void* vac_o_sux;        // +0x0C8
	void* speedcylinder;    // +0x0CC
	void* bonuspop;         // +0x0D0
	void* buzzbonus;        // +0x0D4
	void* breakbridge;      // +0x0D8
	void* unlock;           // +0x0DC
	void* NeonRide;         // +0x0E0
	void* NeonFlicker;      // +0x0E4
	void* ZoopDown;         // +0x0E8
	void* LightsOff;        // +0x0EC
	void* GlassBonus;       // +0x0F0 
};
#pragma pack(pop)

#pragma pack(push, 1)
/// @brief This contains the different fonts you can use for custom text. WARNING: if you try to get a reference to this in your mod's initalize function,
/// the font will likely not be initalized yet, giving you an invalid reference. 
struct Fonts {
	void* showcardGothic28;		// +0x0  (App+0x318)
	void* showcardGothic14;		// +0x4
	void* showcardGothic16;		// +0x8
	void* arialNarrow12bold;	// +0xC
	void* showcardGothic72;		// +0x10 ONLY HAS 0-9
};
#pragma pack(pop)

#pragma pack(push, 1)
/// @brief A struct representing the game's App object. This contains a lot of fields about basic game things such as settings
/// and unlocks. 
/// Fields labeled unverifed mean that the field may be wrong.
/// Keep in mind that not only is the struct likely missing some fields, but some of them cannot be edited in the way 
/// that you may hope. Some can only be read, while others will allow you to change the value. 
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
	std::uint8_t pad_178[0x17C - 0x178];
	void* audioSystem; // +0x17C unverified
	void* inputHandler; // +0x180 unverified
	void* gameUpdateObj; // +0x184 unverified
	std::uint8_t pad_188[0x238 - 0x188];
	bool rightButtonPauseEnabled; // +0x238
	std::uint8_t pad_239[0x318 - 0x239];
	Fonts fonts; // +0x318
	std::uint8_t pad_32C[0x43C - 0x32C];
	Sounds sounds; // +0x43C
	std::uint8_t pad_530[0x534 - 0x530];
	void* musicHandle; // +0x534 unverified
	void* musicChannel1; // +0x538 unverified
	void* musicChannel2; // +0x53C unverified
	std::uint8_t pad_540[0x10];

	// take these ones with a massive grain of salt, i'm just mapping them out for good measure
	void* gameMode1; // +0x550 unverified; something about 1 player mode object
	void* gameMode2; // +0x554 unverified; something about 2 player mode object
	void* gameMode3; // +0x558 unverified; something about 4 player mode object
	void* gameMode4; // +0x55C unverified; something about tournament mode object

	std::uint8_t pad_560[0x84C - 0x560];
	float sensitivity; // +0x84C
	bool unlockMirrorTournament;// +0x850
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



// These are just to ensure I did the structs right, don't mind these 
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
static_assert(offsetof(Ball, home_position_x) == 0xC60);
static_assert(offsetof(Ball, home_distance) == 0xC6C);
static_assert(offsetof(Ball, chase_distance) == 0xC70);
static_assert(offsetof(Ball, is_badball_on_screen) == 0xC74);
static_assert(offsetof(Ball, spin_counter) == 0xC78);
static_assert(offsetof(Ball, spin_distance) == 0xC7C);

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
static_assert(offsetof(PhysicsObject, collision_arr) == 0x424);
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
static_assert(sizeof(Fonts) == 0x14);
static_assert(offsetof(App, fonts) == 0x318);
static_assert(offsetof(App, sounds) == 0x43C);
static_assert(sizeof(Sounds) == 0xF4);
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
static_assert(offsetof(Scene, collision_mesh) == 0x8B0);
static_assert(offsetof(Scene, camera_angle) == 0x29BC);
static_assert(offsetof(Scene, camera_distance) == 0x29C0);
static_assert(offsetof(Scene, current_ball_ptr) == 0x29D0);
static_assert(offsetof(Scene, ball_list_count) == 0x29D8);
static_assert(offsetof(Scene, ball_array) == 0x2DE0);
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
static_assert(offsetof(Scene, arena_timer) == 0x47AC);
static_assert(offsetof(Scene, timer_started) == 0x47B0);
static_assert(offsetof(Scene, p1Score) == 0x47B4);
static_assert(offsetof(Scene, p2Score) == 0x47B8);
static_assert(offsetof(Scene, p3Score) == 0x47BC);
static_assert(offsetof(Scene, p4Score) == 0x47C0);
static_assert(offsetof(Scene, weird_camera) == 0x47C4);
static_assert(offsetof(Scene, is_tiebreaker) == 0x47C5);

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
