#include "HamsterballAPI.h"
#include <windows.h>
#include <fstream>
#include <set>
#include <ctime>
#include <cstring>

typedef void(__thiscall* currFunc)(void* this_ptr, float param_1, float param_2, float param_3);
currFunc Original_currFunc = nullptr;

class PrintArgs : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
    std::set<void*> playerIds; 
    inline static bool hasPrinted = false;
public:
    const char* GetModName() override { return "Print Args"; }
    const char* GetAuthorName() override { return "arti"; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;
        DWORD baseAddr = api->GetGameBaseAddress();
        //api->PatchMemory(baseAddr + 0x65447, "\x00", 1); // show event planes 
        
        //api->RegisterCustomHook((baseAddr + 0x0C5D0), &Hooked_currFunc, (void**)&Original_currFunc);
        //api->PatchMemory(baseAddr + 0x29d23, "\x01", 1);
        //api->PatchMemory(baseAddr + 0x29d0a, "\x01", 1);
        api->RegisterCustomControl("fly", DIK_W);

        // playsound3d
        //api->RegisterCustomHook(baseAddr + 0x59860, &Hooked_currFunc, (void**)&Original_currFunc);
    }

    static void __fastcall Hooked_currFunc(void* this_ptr, void* edx_dummy, float param_1, float param_2, float param_3) {
        //printf("this_ptr: %x | param_1: %f | param_2: %f | param_3: %f\n", this_ptr, param_1, param_2, param_3);
        Original_currFunc(this_ptr, param_1, param_2, param_3);
    }

    void onEventPlaneCollide(Ball* colliding_ball, char* eventPlaneID) override {
        if (strcmp(eventPlaneID, "E:CAMLEFT") == 0) {
            colliding_ball->burn_amount = 1.0f;
            float* camera_angle = &api->GetScene()->camera_angle;
            if (*camera_angle == 90) {
                *camera_angle = 180;
            }
            else {
                *camera_angle = 90;
            }
        }
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
            /*App* app = api->GetApp();
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
            }*/
            printf("base addr: %x\n", api->GetGameBaseAddress());
            printf("app: %x\n", api->GetApp()); 
            printf("scene: %x\n", api->GetScene());
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

        if (api->IsControlDown("fly")) {
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
        if (api->WasKeyPressed(DIK_R)) {
            api->ReloadIniFile(); 
        }

        if (api->WasKeyPressed(DIK_S)) {
            api->ApplyForce(player, 0.0f, 1.0f, 0.0f, 1000);
        }
        if (api->WasKeyPressed(DIK_A)) {
            CallMethod(0x016F0, player, 0.0f, 1.0f, 0.0f, 1000.0f);
        }

        if (api->WasKeyPressed(DIK_G)) {
            api->GetPhysicsObj()->gravity_x = 1; 
        }
        if (api->WasKeyPressed(DIK_E)) {
            CallMethod(0x029C0, api->GetPhysicsObj(), 1.0f);
        }
        if (api->IsKeyDown(DIK_T)) {
            api->GetPlayer()->playerID = -1; 
            CallFast(0x08390, api->GetPlayer());
        }
        if (api->IsKeyDown(DIK_D)) {
            api->SetSpeed(player, 10.0f);
        }

        if (api->WasKeyPressed(DIK_M)) {
            size_t enemyCount;
            Ball* enemy = api->GetEnemies(&enemyCount)[0]; 
            enemy->radius = 100.0f;
        }

        if (api->WasKeyPressed(DIK_F)) {
            api->GetPhysicsObj()->velocity_y = 100.0f;
        }
        DWORD baseAddr = api->GetGameBaseAddress();
        //if (api->WasKeyPressed(DIK_9)) {
        //    //api->PatchMemory(baseAddr + 0x08412, "\x83\x7F\x18\x01", 4);
        //    api->PatchMemory(baseAddr + 0x08416, "\x0F\x85\xAA", 3);
        //}
        //if (api->WasKeyPressed(DIK_0)) {
        //    //api->PatchMemory(baseAddr + 0x08412, "\x83\x7F\x18\xFF", 4);
        //    api->PatchMemory(baseAddr + 0x08416, "\x0F\x84\xAA", 3);
        //}


        if (api->WasKeyPressed(DIK_5)) {
            Ball* p = api->GetPlayer();
            Vec3 pos = Vec3((p->pos_x + 50), (p->pos_y), (p->pos_z + 50));
            Vec3 home_pos = Vec3((p->pos_x + 50), (p->pos_y), (p->pos_z + 50));
            api->CreateBadBall(pos, home_pos);
        }

        if (api->WasKeyPressed(DIK_6)) {
            CallMethod(0x19FA0, api->GetScene(), api->GetPlayer(), false);
        }
        if (api->WasKeyPressed(DIK_7)) {
            CallMethod(0x19FA0, api->GetScene(), api->GetPlayer(), true);
        }
        if (api->WasKeyPressed(DIK_8)) {
            size_t size;
            CallMethod(0x19FA0, api->GetScene(), (Ball*)api->GetEnemies(&size), false);
        }
        if (api->WasKeyPressed(DIK_9)) {
            DWORD baseAddr = api->GetGameBaseAddress();
            if (*(bool*)(baseAddr + 0x1a394) == 1) {
                api->PatchMemory(api->GetGameBaseAddress() + 0x1a394, "\x00", 1);
            }
            else {
                api->PatchMemory(api->GetGameBaseAddress() + 0x1a394, "\x01", 1);
            }
            
        }
        if (api->IsKeyDown(DIK_NUMPAD7)) {
            api->GetScene()->camera_angle += .8;
        }
        if (api->IsKeyDown(DIK_NUMPAD9)) {
            api->GetScene()->camera_angle -= .8;
        }
        if (api->IsKeyDown(DIK_EQUALS)) {
            api->GetScene()->camera_distance -= 5;
        }
        if (api->IsKeyDown(DIK_MINUS)) {
            api->GetScene()->camera_distance += 5;
        }
        if (api->IsKeyDown(DIK_NUMPAD8)) {
            api->GetScene()->cam_offset_y += 2;
        }
        if (api->IsKeyDown(DIK_NUMPAD5)) {
            api->GetScene()->cam_offset_y -= 2;
        }
        if (api->IsKeyDown(DIK_NUMPAD4)) {
            api->GetScene()->cam_offset_x -= 2;
            api->GetScene()->cam_offset_z -= 2;
        }
        if (api->IsKeyDown(DIK_NUMPAD6)) {
            api->GetScene()->cam_offset_x += 2;
            api->GetScene()->cam_offset_z += 2;
        }
        if (api->IsKeyDown(DIK_NUMPAD1)) { // collision 
            if (*(bool*)(baseAddr + 0x65440) == 1) {
                api->PatchMemory(baseAddr + 0x65440, "\x00", 1);
            }
            else {
                api->PatchMemory(baseAddr + 0x65440, "\x01", 1);
            }
        }
        if (api->IsKeyDown(DIK_NUMPAD2)) { // visibility 
            if (*(bool*)(baseAddr + 0x61B60) == 1) {
                api->PatchMemory(baseAddr + 0x61B60, "\x00", 1);
            }
            else {
                api->PatchMemory(baseAddr + 0x61B60, "\x01", 1);
            }
        }
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new PrintArgs();
}