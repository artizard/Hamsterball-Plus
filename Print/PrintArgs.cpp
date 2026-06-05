#include "HamsterballAPI.h"
#include <windows.h>
#include <fstream>

typedef bool(__thiscall* currFunc)(void* it, float param_1);
currFunc Original_currFunc = nullptr;

class PrintArgs : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
public:
    const char* GetModName() override { return "Print Args"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;

        /*DWORD baseAddr = api->GetGameBaseAddress();
        api->RegisterCustomHook((baseAddr + 0x029C0), &Hooked_currFunc, (void**)&Original_currFunc);*/
    }

    static void __fastcall Hooked_currFunc(void* it, float param_1) {
        std::ofstream logFile("C:\\Users\\artiz\\Documents\\hbModStuff\\loader_log.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << "player:     0x" << std::hex << api->GetPlayer() << std::dec << "\n";
            logFile << "physics object:     0x" << std::hex << api->GetPhysicsObj() << std::dec << "\n";
            logFile << "it:     0x" << std::hex << it << std::dec << "\n";
            logFile << "param_1: " << param_1 << "\n";
            logFile << "-----------------------------------------\n";
            logFile.close();
        }
        Original_currFunc(it, param_1);
    }

    void onGameUpdate() override {
        // p for print
        if (api->WasKeyPressed(DIK_P)) {
            App* app = api->GetApp();
            std::ofstream logFile("C:\\Users\\artiz\\Documents\\hbModStuff\\loader_log.txt", std::ios::app);
            if (logFile.is_open()) {
                logFile << "App hex:     0x" << std::hex << app << std::dec << "\n";
                logFile << "player:     0x" << std::hex << api->GetPlayer() << std::dec << "\n";
                logFile << "physics object:     0x" << std::hex << api->GetPhysicsObj() << std::dec << "\n";
                logFile << "isFullscreen: " << ((app->isFullscreen) ? "True" : "False") << "\n";
                logFile << "quitFlag: " << ((app->quitFlag) ? "True" : "False") << "\n";
                logFile << "isGameFocused: " << ((app->isGameFocused) ? "True" : "False") << "\n";
                logFile << "width: " << app->gameWidth << "\n";
                logFile << "height: " << app->gameHeight << "\n";
                logFile << "rightButtonPauseEnabled: " << ((app->rightButtonPauseEnabled) ? "True" : "False") << "\n";
                logFile << "sensitivity: " << app->sensitivity << "\n";
                logFile << "-----------------------------------------\n";
                logFile.close();
            }
        }
        
        // q for quit 
        if (api->WasKeyPressed(DIK_Q)) {
            api->QuitGame(); 
        }
        // u for unlock all
        if (api->WasKeyPressed(DIK_U)) {
            api->UnlockAll();
        }
        // l for lock all
        if (api->WasKeyPressed(DIK_L)) {
            api->LockAll();
        }
        void* player = api->GetPlayer(); 

        if (api->WasKeyPressed(DIK_W)) {
            api->ApplyForce(player, 0.0f, 1.0f, 0.0f, 1000);
        }
        if (api->WasKeyPressed(DIK_S)) {
            api->ApplyForce(player, 0.0f, -1.0f, 0.0f, 1000);
        }

        if (api->WasKeyPressed(DIK_U)) {
            api->GetPhysicsObj()->gravity_x = 1; 
        }
        if (api->WasKeyPressed(DIK_E)) {
            CallMethod(0x029C0, api->GetPhysicsObj(), 1000.0f);
        }

        if (api->WasKeyPressed(DIK_M)) {
            api->GetPlayer()->radius = 2.0f;
        }

        if (api->WasKeyPressed(DIK_F)) {
            api->GetPhysicsObj()->velocity_y = 100.0f;
        }

    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new PrintArgs();
}