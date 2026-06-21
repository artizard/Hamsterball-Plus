#include "HamsterballAPI.h"
#include <windows.h>
#include <random>
class DVDlogo : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
    CustomText dvdLogo; 
    App* app; 
    float velX;
    float velY;
    float posX; // Storing float version to have more precision
    float posY; 
public:
    const char* GetModName() override { return "DVD Logo Bounce"; }
    const char* GetAuthorName() override { return "arti"; }

    int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;
        app = api->GetApp(); 

        CustomButton onButton("DVD_BOUNCE_ON", "TOGGLE DVD BOUNCE");
        api->CreateToggleButton(onButton, this);

        dvdLogo.text = "HB2";

        posX = dvdLogo.x = 523;
        posY = dvdLogo.y = 492;
        velX = 1.5;
        velY = 1.5;
        changeLogoColor(); 
    }

    void onTextRenderLoop() override {
        if (api->GetButtonState("DVD_BOUNCE_ON")) {
            dvdLogo.font = app->fonts.showcardGothic28; // had to do this here because fonts are not initialized in time for the mod's Initalize function 
            if (dvdLogo.x < 65 && velX < 0) {
                velX *= -1;
                changeLogoColor();
            }
            else if (dvdLogo.x > 800 && velX > 0) {
                velX *= -1;
                changeLogoColor();
            }
            else if (dvdLogo.y < -7 && velY < 0) {
                velY *= -1;
                changeLogoColor();
            }
            else if (dvdLogo.y > 555 && velY > 0) {
                velY *= -1;
                changeLogoColor();
            }
            api->DrawCustomText(dvdLogo);

            posX += velX;
            posY += velY;
            dvdLogo.x = (int)posX;
            dvdLogo.y = (int)posY;
        }
    }
    void changeLogoColor() {
        std::random_device rd; 
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(0, 255);
        dvdLogo.text_color.r = distrib(gen);
        dvdLogo.text_color.g = distrib(gen);
        dvdLogo.text_color.b = distrib(gen);
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new DVDlogo();
}