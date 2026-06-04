#include "HamsterballAPI.h"
#include <windows.h>
#include <fstream>

typedef bool(__fastcall* currFunc)(void* app);
currFunc Original_currFunc = nullptr;

class PrintArgs : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Print Args"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        DWORD baseAddr = api->GetGameBaseAddress();
        api->RegisterCustomHook((baseAddr + 0x6bd80), &Hooked_currFunc, (void**)&Original_currFunc);
    }

    static void __fastcall Hooked_currFunc(void* app) {
        
        std::ofstream logFile("C:\\Users\\artiz\\Documents\\loader_log.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << "App hex:     0x" << std::hex << (DWORD)app << std::dec << "\n";
            logFile << "-----------------------------------------\n";
            logFile.close();
        }
        Original_currFunc(app);
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new PrintArgs();
}