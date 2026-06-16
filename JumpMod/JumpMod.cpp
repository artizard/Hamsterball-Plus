#include "HamsterballAPI.h"
#include <windows.h>
class JumpCheat : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Jump Cheat"; }
    const char* GetAuthorName() override { return "arti"; }
    const char* GetContributors() override { return "BookwormKevin"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton jumpButton("CHEAT_JUMP", "JUMPING");
        api->CreateToggleButton(jumpButton);

        api->RegisterCustomControl("jump", DIK_LSHIFT);

        CustomSlider jumpHeightSlider("JUMP_HEIGHT", "JUMP HEIGHT", 10.0f);
        jumpHeightSlider.stepSize = .25;
        jumpHeightSlider.lowerBound = 0;
        api->CreateSlider(jumpHeightSlider);
    }

    void onPlayerUpdate(Ball* playerObject) override {
        if (api->GetButtonState("CHEAT_JUMP")) {
            if (api->WasControlPressed("jump")) {
                PhysicsObject* physicsObj = playerObject->physics_object; 

                if (physicsObj != nullptr) {

                    // Read Y Velocity
                    float trueVelY = physicsObj->velocity_y;

                    // Ground Check
                    float tolerance = 0.5f;

                    if (trueVelY > -tolerance && trueVelY < tolerance) {
                        // Apply the jump force
                        //physicsObj->velocity_y = 20.0f; 
                        physicsObj->velocity_y = api->GetSliderState("JUMP_HEIGHT") * 2; 
                    }
                }
            }
            if (api->WasKeyPressed(DIK_W)) {
                PhysicsObject* physicsObj = playerObject->physics_object;

                if (physicsObj != nullptr) {

                    // Read Y Velocity
                    float trueVelY = physicsObj->velocity_y;

                    // Ground Check
                    float tolerance = 0.5f;

                    if (trueVelY > -tolerance && trueVelY < tolerance) {
                        // Apply the jump force
                        api->ApplyForce(playerObject, 0, 1.0f, 0, 100);
                    }
                }
            }
            if (api->WasKeyPressed(DIK_E)) {
                PhysicsObject* physicsObj = playerObject->physics_object;

                if (physicsObj != nullptr) {

                    // Read Y Velocity
                    float trueVelY = physicsObj->velocity_y;

                    // Ground Check
                    float tolerance = 0.5f;

                    if (trueVelY > -tolerance && trueVelY < tolerance) {
                        // Apply the jump force
                        api->ApplyForce(playerObject, 0, .8, 0, 100);
                    }
                }
            }

        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new JumpCheat();
}