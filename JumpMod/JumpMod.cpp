#include "HamsterballAPI.h"
#include <windows.h>
class JumpCheat : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Jump Cheat"; }
    const char* GetAuthorName() override { return "arti"; }
    const char* GetContributors() override { return "BookwormKevin"; }
    int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton jumpButton("CHEAT_JUMP", "JUMPING");
        jumpButton.trueText = "ON";
        jumpButton.falseText = "OFF"; 
        api->CreateToggleButton(jumpButton, this);

        api->RegisterCustomControl("jump", DIK_LSHIFT);

        CustomSlider jumpHeightSlider("JUMP_HEIGHT", "JUMP HEIGHT", 10.0f);
        jumpHeightSlider.stepSize = .25;
        jumpHeightSlider.lowerBound = 0;
        api->CreateSlider(jumpHeightSlider, this);
    }

    void onPlayerUpdate(Ball* playerObject) override {
        if (playerObject->playerID != 0) return; // only affect player 1
        if (api->GetButtonState("CHEAT_JUMP")) { // if cheat_jump is enabled
            if (api->WasControlPressed("jump")) { // if the jump control was pressed 
                PhysicsObject* physicsObj = playerObject->physics_object; 
                Sounds sounds = api->GetApp()->sounds;
                if (physicsObj == nullptr) return; 
                
                // I'm using the raycast function to determine if the player is grounded or not. If the play is grounded, then they can jump,
                // if they aren't then they can't. I'm doing it in the direction of the player's gravity to make this work with different 
                // gravities (like in odd race). This means that the ray will always shoot out in the direction of the ground. If the player is grounded,
                // I apply an upwards velocity in all the directions that there is gravity. I chose to do this in all directions in case someone makes
                // a level with gravity in multiple directions. 
                Vec3 playerPos = Vec3(playerObject->pos_x, playerObject->pos_y, playerObject->pos_z);
                Vec3 playerGravity(physicsObj->gravity_x, physicsObj->gravity_y, physicsObj->gravity_z); 
                bool isGrounded = api->LevelRaycastHit(playerPos, playerGravity, 26.0f, 35);
                if (isGrounded) {
                    api->PlaySoundEffect(sounds.bubble1, 1.0f);
                    float jumpHeight = api->GetSliderState("JUMP_HEIGHT");
                    if (physicsObj->gravity_x) physicsObj->velocity_x = -physicsObj->gravity_x * jumpHeight * 2;
                    if (physicsObj->gravity_y) physicsObj->velocity_y = -physicsObj->gravity_y * jumpHeight * 2;
                    if (physicsObj->gravity_z) physicsObj->velocity_z = -physicsObj->gravity_z * jumpHeight * 2;
                }
                
            }
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new JumpCheat();
}