#include "HamsterballAPI.h"
#include <windows.h>
class REPLACE_WITH_YOUR_MOD_NAME : public HamsterballAPI {
private:
    IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "PUT YOUR MOD NAME HERE"; }
    const char* GetAuthorName() override { return "PUT YOUR NAME HERE"; }

    int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton jumpButton("tEST", "TEST");
        api->CreateToggleButton(jumpButton, this);
    }

};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new REPLACE_WITH_YOUR_MOD_NAME();
}