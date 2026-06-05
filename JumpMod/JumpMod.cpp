#include "HamsterballAPI.h"
#include <windows.h>
class JumpCheat : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Jump Cheat"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;
        api->CreateToggleButton("CHEAT_JUMP", "JUMPING", false);
    }

    void onPlayerUpdate(void* playerObject) override {
        if (api->GetButtonState("CHEAT_JUMP")) {
            if (api->WasKeyPressed(DIK_LSHIFT)) {
                DWORD* physicsObjPtr = (DWORD*)((DWORD)playerObject + 0x1a4);

                if (physicsObjPtr != nullptr && *physicsObjPtr != 0) {

                    DWORD physicsObj = *physicsObjPtr;

                    // Read Y Velocity
                    float* trueVelY = (float*)(physicsObj + 0xca8);

                    // Ground Check
                    float tolerance = 0.5f;

                    if (*trueVelY > -tolerance && *trueVelY < tolerance) {
                        // Apply the jump force
                        *trueVelY = 20.0f;
                    }
                }
            }
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new JumpCheat();
}