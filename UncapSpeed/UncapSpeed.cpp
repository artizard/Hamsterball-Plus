#include "HamsterballAPI.h"
#include <windows.h>
class UncapSpeed : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Uncap Speed"; }
    const char* GetAuthorName() override { return "arti"; }
    const char* GetContributors() override { return "BookwormKevin"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton speedButton("CHEAT_SPEED", "UNCAP SPEED");
        api->CreateToggleButton(speedButton, this);
    }

    void onPlayerUpdate(Ball* playerObject) override {
        if (api->GetButtonState("CHEAT_SPEED")) {
            float* masterSpeed = (float*)((DWORD)playerObject + 0x188);
            *masterSpeed = 500.0f;

            DWORD* physicsObjPtr = (DWORD*)((DWORD)playerObject + 0x1a4);
            if (physicsObjPtr != nullptr && *physicsObjPtr != 0) {
                DWORD physicsObj = *physicsObjPtr;

                // Break the global physics speed cap
                float* maxSpeed = (float*)(physicsObj + 0xc70);
                *maxSpeed = 9999.0f;

                // Turn off air resistance
                float* drag = (float*)(physicsObj + 0xc68);
                *drag = 0.0f;
            }
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new UncapSpeed();
}