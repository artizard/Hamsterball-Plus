#include "HamsterballAPI.h"
#include <windows.h>
#include <fstream>
#include <set>

typedef bool(__fastcall* currFunc)(int* param_1);
currFunc Original_currFunc = nullptr;

class PrintArgs : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
    std::set<void*> playerIds; 
public:
    const char* GetModName() override { return "Print Args"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;
        /*DWORD baseAddr = api->GetGameBaseAddress();
        api->RegisterCustomHook((baseAddr + 0x19C00), &Hooked_currFunc, (void**)&Original_currFunc);*/
    }

    static void __fastcall Hooked_currFunc(int* param_1) {
        std::ofstream logFile("C:\\Users\\artiz\\Documents\\hbModStuff\\loader_log.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << "param_1: " << std::hex << param_1 << std::dec << "\n";
            logFile << "-----------------------------------------\n";
            logFile.close();
        }
        Original_currFunc(param_1);
    }

    //void onPlayerUpdate(Ball* ball) override {
    //    int playerId = ball->playerID;
    //    if (!playerIds.count(ball)) {
    //        playerIds.insert(ball);
    //        std::ofstream logFile("C:\\Users\\artiz\\Documents\\hbModStuff\\loader_log.txt", std::ios::app);
    //        if (logFile.is_open()) {
    //            logFile << "Ball hex:     0x" << std::hex << ball << std::dec << "\n";
    //            logFile << "ball id: " << playerId << "\n";
    //            logFile << "scene: 0x" << std::hex << ball->scene << std::dec << "\n";
    //            logFile << "-----------------------------------------\n";
    //            logFile.close();
    //        }
    //    }
    //}

    void onGameUpdate() override {
        // p for print
        if (api->WasKeyPressed(DIK_P)) {
            App* app = api->GetApp();
            std::ofstream logFile("C:\\Users\\artiz\\Documents\\hbModStuff\\loader_log.txt", std::ios::app);
            if (logFile.is_open()) {
                logFile << "App hex:     0x" << std::hex << app << std::dec << "\n";
                logFile << "player1:     0x" << std::hex << api->GetPlayer() << std::dec << "\n";
                logFile << "player2:     0x" << std::hex << api->GetPlayer2() << std::dec << "\n";
                logFile << "player3:     0x" << std::hex << api->GetPlayer3() << std::dec << "\n";
                logFile << "player4:     0x" << std::hex << api->GetPlayer4() << std::dec << "\n";
                size_t enemyCount;
                Ball** enemies = api->GetEnemies(&enemyCount);
                for (int i = 0; i < enemyCount; i++) {
                    logFile << "enemy" << (i+1) <<":     0x" << std::hex << enemies[i] << std::dec << "\n";
                }
                
                logFile << "physics object:     0x" << std::hex << api->GetPhysicsObj() << std::dec << "\n";
                logFile << "scene:     0x" << std::hex << api->GetScene() << std::dec << "\n";
                logFile << "scene app:     0x" << std::hex << api->GetScene()->owner_app << std::dec << "\n";
                logFile << "ball scene:     0x" << std::hex << api->GetPlayer()->scene << std::dec << "\n";
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
        Ball* player = api->GetPlayer(); 

        if (api->IsKeyDown(DIK_W)) {
            api->GetPhysicsObj()->velocity_y = 2.0f;
        }
        if (api->IsKeyDown(DIK_1)) {
            api->GetPlayer()->pos_y = 600.0f;
        }
        if (api->IsKeyDown(DIK_2)) {
            api->GetPlayer()->pos_y = 625.0f;
        }
        if (api->IsKeyDown(DIK_3)) {
            api->GetPlayer()->pos_y = 650.0f;
        }

        if (api->WasKeyPressed(DIK_S)) {
            api->ApplyForce(player, 0.0f, -1.0f, 0.0f, 1000);
        }

        if (api->WasKeyPressed(DIK_G)) {
            api->GetPhysicsObj()->gravity_x = 1; 
        }
        if (api->WasKeyPressed(DIK_E)) {
            CallMethod(0x029C0, api->GetPhysicsObj(), 1.0f);
        }
        if (api->IsKeyDown(DIK_D)) {
            api->SetSpeed(player, 10.0f);
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