#include "HamsterballAPI.h"
#include <windows.h>
#include <stdio.h>

typedef void(__thiscall* Shatter1_t)(Ball* ball, int param_1);
typedef void(__fastcall* Shatter2_t)(Ball* ball);
typedef void(__fastcall* Shatter3_t)(Ball* ball);

Shatter1_t Original_Shatter1 = nullptr;
Shatter2_t Original_Shatter2 = nullptr;
Shatter3_t Original_Shatter3 = nullptr;

class NoBreak : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "No Break"; }
    const char* GetAuthorName() override { return "arti"; }
    const char* GetContributors() override { return "XRow, BookwormKevin"; }
    int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        CustomButton noBreakButton("CHEAT_NOBREAK", "NO BREAK");
        api->CreateToggleButton(noBreakButton, this);

        
        DWORD baseAddr = api->GetGameBaseAddress();
        api->RegisterCustomHook((baseAddr + 0x8D70), &Hooked_Shatter1, (void**)&Original_Shatter1);
        api->RegisterCustomHook((baseAddr + 0x9050), &Hooked_Shatter2, (void**)&Original_Shatter2);
        api->RegisterCustomHook((baseAddr + 0x9480), &Hooked_Shatter3, (void**)&Original_Shatter3);
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

    static void __fastcall Hooked_Shatter1(Ball* ball, void* edx_dummy, int param_1) {
        if (api->GetButtonState("CHEAT_NOBREAK") && ball == api->GetPlayer()) return;
        Original_Shatter1(ball, param_1);
    }

    static void __fastcall Hooked_Shatter2(Ball* ball) {
        if (api->GetButtonState("CHEAT_NOBREAK") && ball == api->GetPlayer()) return;
        Original_Shatter2(ball);
    }

    static void __fastcall Hooked_Shatter3(Ball* ball) {
        if (api->GetButtonState("CHEAT_NOBREAK") && ball == api->GetPlayer()) return;
        Original_Shatter3(ball);
    }

};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new NoBreak();
}