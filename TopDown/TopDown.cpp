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

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton topdownButton("CHEAT_TOPDOWN", "TOP-DOWN");
        api->CreateToggleButton(topdownButton);
    }

    void onRenderApply(void* this_ptr, float* viewMatrix) override {
        void* player = api->GetPlayer();
        if (player != nullptr && api->GetButtonState("CHEAT_TOPDOWN")) {
            // Get Hamster pos
            float pX = *(float*)((DWORD)player + 0x158);
            float pY = *(float*)((DWORD)player + 0x15c);
            float pZ = *(float*)((DWORD)player + 0x160);

            // Build top-down camera angle
            Vec3 target(pX, pY, pZ);
            Vec3 eye(pX, pY + 800.0f, pZ);
            Vec3 up(0.0f, 0.0f, 1.0f);

            // Create a custom 16-float array and do the math
            float customMatrix[16];
            BuildCustomViewMatrix(customMatrix, eye, target, up);

            // copy new matrix into location of old view matrix 
            std::copy(std::begin(customMatrix), std::end(customMatrix), viewMatrix);
        }
    }

    void onEventPlaneCollide(Ball* colliding_ball, char* eventPlaneID) {
        printf("EventPlaneID: %s\n", eventPlaneID);
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new TopDown();
}