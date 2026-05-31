#pragma once
class HamsterballAPI {
public: 
	virtual ~HamsterballAPI() {}

	virtual const char* GetModName() = 0;

	virtual void onPlayerUpdate(void* PlayerObject) {}
};

typedef HamsterballAPI* (*CreateModFunct)(); 