#include "HamsterballAPI.h"
#include <windows.h>
#include <algorithm>
#include <iterator>

class TopDown : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Top Down"; }
    const char* GetAuthorName() override { return "arti"; }
    const char* GetContributors() override { return "BookwormKevin"; }
    int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton topdownButton("CHEAT_TOPDOWN", "TOP-DOWN");
        api->CreateToggleButton(topdownButton, this);

        CustomSlider heightSlider("TOPDOWN_CAM_HEIGHT", "TOP-DOWN CAMERA HEIGHT", 8);
        heightSlider.stepSize = .5;
        heightSlider.decimalPlaces = 1;
        heightSlider.lowerBound = .5;
        api->CreateSlider(heightSlider, this);
    }

    void onRenderApply(void* this_ptr, float* viewMatrix) override {
        Ball* player = api->GetPlayer();
        if (player != nullptr && api->GetButtonState("CHEAT_TOPDOWN")) {
            // Get Hamster pos
            float pX = player->pos_x;
            float pY = player->pos_y;
            float pZ = player->pos_z;
            PhysicsObject* physics = player->physics_object;
            float height = api->GetSliderState("TOPDOWN_CAM_HEIGHT") * 100.0f; 

            // building view matrix
            Vec3 target(pX, pY, pZ);
            Vec3 eye(pX + height * -physics->gravity_x, pY + height * -physics->gravity_y, pZ + height * -physics->gravity_z);
            // up has to be different than gravity, otherwise when it does the vector math it does the cross product of two parallel vectors which
            // gives 0, which breaks the camera.
            Vec3 up(-physics->gravity_z, -physics->gravity_x, -physics->gravity_y);

            // actually build the matrix 
            float customMatrix[16];
            BuildCustomViewMatrix(customMatrix, eye, target, up);

            // copy new matrix into location of old view matrix 
            std::copy(std::begin(customMatrix), std::end(customMatrix), viewMatrix);
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new TopDown();
}