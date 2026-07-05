#pragma once
#include "HamsterballAPI.h"
#include <string>
#include <map>
#include <variant>

struct ButtonData {
	std::string displayText;
	std::string trueText;
	std::string falseText;
	Color color;
	bool isOn;
	HamsterballAPI* owner;
};
struct SliderData {
	std::string displayText;
	float value;
	float stepSize;
	int decimalPlaces;
	float lowerBound;
	float upperBound;
	std::string unitName; 
	Color color;
	HamsterballAPI* owner;
};
using ConfigValue = std::variant<int, float, bool, std::string>;

class ModAPI : public IModAPI {
public:
	std::map<std::string, ButtonData, std::less<>> optionButtons;
	std::map<std::string, SliderData, std::less<>> optionSliders;
	// using std::less<> here to be able to look up controls from the map without converting char* to string, which requires memory allocation (optimization thing)
	std::map<std::string, CustomControl, std::less<>> customControls; // <controlID, CustomControl struct> 
	std::map<std::string, ConfigValue, std::less<>> modConfig;

	void RegisterCustomHook(DWORD targetAddress, void* hookFunction, void** original) override;
	void RegisterCustomControl(const char* controlID, CustomControl defaultControl) override;
	CustomControl GetCustomControlKey(const char* controlID) override;
	bool IsKeyDown(int dik) override;
	bool WasKeyPressed(int dik) override;
	bool WasKeyReleased(int dik) override;
	bool IsControlDown(const char* controlID) override;
	bool WasControlPressed(const char* controlID) override;
	bool WasControlReleased(const char* controlID) override;
	void CreateToggleButton(const CustomButton& button, HamsterballAPI* modInstance) override;
	void CreateSlider(const CustomSlider& slider, HamsterballAPI* modInstance) override;
	PhysicsObject* GetPhysicsObj() override;
	void PatchMemory(DWORD address, const char* bytes, size_t size) override;
	void UnlockAll() override;
	void LockAll() override;
	bool QuitGame() override;
	bool SaveConfig() override;
	void ApplyForce(Ball* player, float x, float y, float z, float magnitude) override;
	void SetSpeed(Ball* player, float mult) override;
	
	Ball* GetPlayer() override;
	Ball* GetPlayer2() override;
	Ball* GetPlayer3() override;
	Ball* GetPlayer4() override;
	Ball** GetEnemies(size_t* enemyCount) override;
	DWORD GetGameBaseAddress() override;
	Scene* GetScene() override;
	App* GetApp() override;
	bool GetButtonState(const char* id) override;
	float GetSliderState(const char* id) override;
	void* AllocateMem(unsigned int size) override;
	void CreateBadBall(Vec3 spawn_pos, Vec3 home_pos, float home_distance, float chase_distance, float radius, float spin_distance) override;
	int GetTimerTime() override; // in game timer for time trials and tournament 
	void ReloadIniFile();
	void SetTimerTime(int time) override;
	Vec3 LevelRaycastVec(Vec3 position, Vec3 direction, float radius) override;
	bool LevelRaycastHit(Vec3 position, Vec3 direction, float radius, float max_dist) override;
	PhysicsConstants* GetPhysicsConstants() override;
	void PlaySoundEffect(void* soundEffect, float volume) override;
	void Play3dSoundEffect(void* soundEffect, Vec3 position, float volume) override;
	void ShowBallMessage(Ball* ball, char* message) override;
	void RespawnPlayer(Ball* player) override;
	void DrawCustomText(const char* text, const CustomText& params) override;
	float GetBallSpeed(Ball* ball) override;
	void ShatterBall(Ball* ball) override;
	void DrawTimedMessage(const char* text, const CustomText& params, float messageDuration) override;
	void RegisterConfigInt(const char* configID, int defaultValue) override;
	void RegisterConfigFloat(const char* configID, float defaultValue) override;
	void RegisterConfigBool(const char* configID, bool defaultValue) override;
	void RegisterConfigString(const char* configID, const char* defaultValue) override;
	int GetConfigInt(const char* configID) override;
	float GetConfigFloat(const char* configID) override;
	bool GetConfigBool(const char* configID) override;
	const char* GetConfigString(const char* configID) override;
private:
	void setUnlocks(bool isUnlock);
};

extern ModAPI g_ModApiInstance;