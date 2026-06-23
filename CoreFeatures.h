#pragma once
#include "HamsterballAPI.h"
#include <windows.h>
/// @brief This contains the buttons and such that I want to be built in with the mod loader. I made this the same way as a mod for simplicity. 
class CoreFeatures : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override;
    const char* GetAuthorName() override;
    int GetApiVersion() override;
    void Initialize(IModAPI* modApi) override;
    void onBallUpdate(Ball* playerObject) override;
};
