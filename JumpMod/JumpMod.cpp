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
        api->CreateToggleButton(jumpButton, this);

        api->RegisterCustomControl("jump", DIK_LSHIFT);

        CustomSlider jumpHeightSlider("JUMP_HEIGHT", "JUMP HEIGHT", 10.0f);
        jumpHeightSlider.stepSize = .25;
        jumpHeightSlider.lowerBound = 0;
        api->CreateSlider(jumpHeightSlider, this);
    }

    void onPlayerUpdate(Ball* playerObject) override {
        if (api->GetButtonState("CHEAT_JUMP")) {
            if (api->WasControlPressed("jump")) {
                PhysicsObject* physicsObj = playerObject->physics_object; 

                if (physicsObj != nullptr) {

                    //// Read Y Velocity
                    //float trueVelY = physicsObj->velocity_y;

                    //// Ground Check
                    //float tolerance = 0.5f;

                    //if (trueVelY > -tolerance && trueVelY < tolerance) {
                    //    // Apply the jump force
                    //    //physicsObj->velocity_y = 20.0f; 
                    //    physicsObj->velocity_y = api->GetSliderState("JUMP_HEIGHT") * 2; 
                    //}
                    Ball* player = api->GetPlayer();
                    Vec3 playerPos = Vec3(player->pos_x, player->pos_y, player->pos_z);
                    bool isGrounded = api->LevelRaycastHit(playerPos, Vec3(0, -1, 0), 26.0f, 15);
                    if (isGrounded) {
                        physicsObj->velocity_y = api->GetSliderState("JUMP_HEIGHT") * 2;
                    }
                }
            }
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new JumpCheat();
}