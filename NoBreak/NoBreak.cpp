#include "HamsterballAPI.h"
#include <windows.h>

typedef void(__thiscall* Shatter1_t)(void* this_ptr, int param_1);
typedef void(__fastcall* Shatter2_t)(void* param_1);
typedef void(__fastcall* Shatter3_t)(void* param_1);

Shatter1_t Original_Shatter1 = nullptr;
Shatter2_t Original_Shatter2 = nullptr;
Shatter3_t Original_Shatter3 = nullptr;

class NoBreak : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "No Break"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;
        api->CreateToggleButton("CHEAT_NOBREAK", "NO BREAK", false);

        DWORD baseAddr = api->GetGameBaseAddress();
        api->RegisterCustomHook((baseAddr + 0x8D70), &Hooked_Shatter1, (void**)&Original_Shatter1);
        api->RegisterCustomHook((baseAddr + 0x9050), &Hooked_Shatter2, (void**)&Original_Shatter2);
        api->RegisterCustomHook((baseAddr + 0x9480), &Hooked_Shatter3, (void**)&Original_Shatter3);
    }

    void onPlayerUpdate(void* playerObject) override {
        if (api->GetButtonState("CHEAT_NOBREAK")) {
            if (api->WasKeyPressed(0x2A)) {
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

    void onButtonToggle(const char* buttonId, bool newState) override {
        if (strcmp(buttonId, "CHEAT_NOBREAK") == 0) {
            ApplyNoFallDamage(newState); 
        }
    }

    void ApplyNoFallDamage(bool enable) {

        DWORD baseAddr = (DWORD)GetModuleHandle(NULL);

        if (enable) {
            // Overwrite with NOPs (0x90)

            // Regular E:LIMIT check
            api->PatchMemory(baseAddr + 0xC761, "\x90\x90\x90\x90\x90\x90", 6);
            api->PatchMemory(baseAddr + 0xC767, "\x90\x90\x90\x90\x90\x90\x90", 7);

            // Odd Race's E:LIMIT check
            api->PatchMemory(baseAddr + 0xF226, "\x90\x90\x90\x90\x90\x90\x90", 7);
            api->PatchMemory(baseAddr + 0xF22D, "\x90\x90\x90\x90\x90\x90\x90", 7);

            // Unknown, inside ballupdate
            api->PatchMemory(baseAddr + 0x75C9, "\x90\x90\x90\x90\x90\x90", 6);

        }
        else {
            // Restore original game code
            api->PatchMemory(baseAddr + 0xC767, "\xC6\x85\xE9\x02\x00\x00\x01", 7);
            api->PatchMemory(baseAddr + 0xF22D, "\xC6\x86\xE9\x02\x00\x00\x01", 7);
            api->PatchMemory(baseAddr + 0x75C9, "\xFF\x86\xEC\x02\x00\x00", 6);
            api->PatchMemory(baseAddr + 0xC761, "\x88\x85\x68\x07\x00\x00", 6);
            api->PatchMemory(baseAddr + 0xF226, "\xC6\x86\x68\x07\x00\x00\x00", 7);
        }
    }

    static void __fastcall Hooked_Shatter1(void* this_ptr, void* edx_dummy, int param_1) {
        if (api->GetButtonState("CHEAT_NOBREAK") && this_ptr == api->GetPlayer()) return;
        Original_Shatter1(this_ptr, param_1);
    }

    static void __fastcall Hooked_Shatter2(void* param_1, void* edx_dummy) {
        if (api->GetButtonState("CHEAT_NOBREAK") && param_1 == api->GetPlayer()) return;
        Original_Shatter2(param_1);
    }

    static void __fastcall Hooked_Shatter3(void* param_1, void* edx_dummy) {
        if (api->GetButtonState("CHEAT_NOBREAK") && param_1 == api->GetPlayer()) return;
        Original_Shatter3(param_1);
    }

};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new NoBreak();
}