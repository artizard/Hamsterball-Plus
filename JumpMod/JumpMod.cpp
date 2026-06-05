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

    void onPlayerUpdate(Ball* playerObject) override {
        if (api->GetButtonState("CHEAT_JUMP")) {
            if (api->WasKeyPressed(DIK_LSHIFT)) {
                PhysicsObject* physicsObj = playerObject->physics_object; 

                if (physicsObj != nullptr) {

                    // Read Y Velocity
                    float trueVelY = physicsObj->velocity_y;

                    // Ground Check
                    float tolerance = 0.5f;

                    if (trueVelY > -tolerance && trueVelY < tolerance) {
                        // Apply the jump force
                        physicsObj->velocity_y = 20.0f; 
                    }
                }
            }
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new JumpCheat();
}