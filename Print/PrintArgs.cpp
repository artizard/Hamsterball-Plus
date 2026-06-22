#include "HamsterballAPI.h"
#include <windows.h>
#include <fstream>
#include <set>
#include <ctime>
#include <cstring>
#include <string>

// This mod is a bunch of random stuff I did for debugging purposes, but you may find it useful for seeing how to 
// use certain functions from the modding API (it is very messy though). Many of the mods/keybinds in here are debug/proof of concepts
// which have no error handling or are unstable so keep that in mind. 

typedef void(__thiscall* currFunc)(void* this_ptr, char* text, int x, int y, int shadowOffsetX, int shadowOffsetY, DWORD vtable, Color c1, DWORD vtable2, Color c2);
currFunc Original_currFunc = nullptr;

typedef void(__fastcall* renderFunc)(void* this_ptr);
renderFunc original_renderFunc; 

class PrintArgs : public HamsterballAPI {
private:
    inline static IModAPI* api = nullptr;
    inline static bool hasPrinted = false;
    inline static bool showText = false;
    inline static int y = 200;
public:

    const char* GetModName() override { return "Print Args"; }
    const char* GetAuthorName() override { return "arti"; }
    int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }

    void Initialize(IModAPI* modApi) override {
        api = modApi;
        DWORD baseAddr = api->GetGameBaseAddress();
        //api->PatchMemory(baseAddr + 0x65447, "\x00", 1); // show event planes 
        
        //api->RegisterCustomHook((baseAddr + 0x0C5D0), &Hooked_currFunc, (void**)&Original_currFunc);
        //api->PatchMemory(baseAddr + 0x29d23, "\x01", 1);
        //api->PatchMemory(baseAddr + 0x29d0a, "\x01", 1);
        api->RegisterCustomControl("fly", DIK_W);
        CustomSlider sizeSlider("HAMSTER_SIZE", "Hamster Size", .037); 
        sizeSlider.stepSize = .01;
        api->CreateSlider(sizeSlider, this);
        // playsound3d
        //api->RegisterCustomHook(baseAddr + 0x59860, &Hooked_currFunc, (void**)&Original_currFunc);
        //api->RegisterCustomHook(baseAddr + 0x09C60, &Hooked_currFunc, (void**)&Original_currFunc);
        //api->RegisterCustomHook(baseAddr + 0x6C250, &Hooked_renderFunc, (void**)&original_renderFunc); 
    }

    static void __fastcall Hooked_currFunc(void* this_ptr, void* edx_dummy, char* text, int x, int y, int shadowOffsetX, int shadowOffsetY, DWORD vtable, Color c1, DWORD vtable2, Color c2) {
        printf("this_ptr: %x, text: %s, x: %d, y: %d, shadowX: %d, shadowY: %d, vtable1: %x, c1: (%f, %f, %f, %f), vtable2: %x, c2: (%f, %f, %f, %f)\n", this_ptr, text, x, y, shadowOffsetX, shadowOffsetY, vtable, c1.r, c1.g, c1.b, c1.a, vtable2, c2.r, c2.g, c2.b, c2.a);
        Original_currFunc(this_ptr, text, x, y, shadowOffsetX, shadowOffsetY, vtable, c1, vtable2, c2);
    }

    static void __fastcall Hooked_renderFunc(void* this_ptr) {
        original_renderFunc(this_ptr);
        if (showText) {
            void* font = *(void**)((char*)api->GetApp() + 0x318);
            Color tc = Color(.5f, .5f, .9f, 1.0f);
            Color sc = Color(0.0f, 0.0f, 0.2f, 1.0f);
            if (font) {
                CallMethod(0x09C60, font, "Test 123", 300, 50, 5, 5, 0, tc.r, tc.g, tc.b, tc.a, 0, sc.r, sc.g, sc.b, sc.a);
            }

        }
    }

    void onTextRenderLoop() override {
        if (showText) {
            CustomText text(api->GetApp()->fonts.arialNarrow12bold, 500, 500, Color(.8f, .2f, .2f, 1.0f), true);
            api->DrawCustomText("TESTING TESTING", text);
        }
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

    void onSliderChange(const char* sliderId, float newValue) override {
        if (strcmp(sliderId, "HAMSTER_SIZE") == 0) {
            api->GetPhysicsConstants()->hamsterSize = newValue; 
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

    // for anyone reading this, note that it is better to do player related keybinds in onPlayerUpdate(). If you do something to the player from this
    // function, you need to make sure the player exists in the first place (it won't during the menu which can cause crashes)
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
            Ball* player = api->GetPlayer();
            PhysicsObject* physics = api->GetPhysicsObj(); 
            printf("base addr: %x\n", api->GetGameBaseAddress());
            printf("app: %x\n", api->GetApp()); 
            printf("scene: %x\n", api->GetScene());
            printf("player 1: %x\n", player);
            printf("player 1 physics: %x\n", api->GetPhysicsObj()); 
            printf("gravity x: %f\n", physics->gravity_x);
            printf("gravity y: %f\n", physics->gravity_y);
            printf("gravity z: %f\n", physics->gravity_z);
            printf("gravity vec: [%f, %f, %f]\n", player->gravity_vec[0], player->gravity_vec[1], player->gravity_vec[2]);
            printf("gravity type: %d\n", player->gravity_type); 
        }
        
        // q for quit 
        /*if (api->WasKeyPressed(DIK_Q)) {
            api->QuitGame(); 
        }*/
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
        if (api->WasKeyPressed(DIK_1)) {
            CallFast(0x59860, api->GetApp()->sounds.whistle, player->pos_x, player->pos_y, player->pos_z, 1.0f);
        }
        if (api->WasKeyPressed(DIK_2)) {
            /*CallFast(0x597b0, (int)api->GetApp()->sounds.whistle);*/
            //CallFast(0x597b0, (int)api->GetApp()->sounds.whistle, 1.0f);
            CallFast(0x59860, api->GetApp()->sounds.whistle, player->pos_x, player->pos_y, player->pos_z, 10.0f);
        }
        if (api->WasKeyPressed(DIK_3)) {
            //printf("Stolen: %d\n", stolenSound);
            ////CallFast(0x597b0, api->GetApp()->sounds.whistle);
            //Original_currFunc(stolenSound);
            size_t num;
            Ball* player = api->GetEnemies(&num)[0];
            api->PlaySoundEffect(api->GetApp()->sounds.whistle, 100.0f);
        }
        if (api->IsKeyDown(DIK_4)) {
            //printf("Stolen: %d\n", stolenSound);
            //Ball* player = api->GetPlayer();
            //CallFast(0x59860, stolenSound, player->pos_x, player->pos_y, player->pos_z);
            ////Original_currFunc(stolenSound);
            size_t num;
            Ball* player = api->GetEnemies(&num)[0]; 
            api->Play3dSoundEffect(api->GetApp()->sounds.whistle, Vec3(player->pos_x, player->pos_y, player->pos_z), 1.0f);
        }

        if (api->WasKeyPressed(DIK_R)) {
            api->ReloadIniFile(); 
        }

        if (api->WasKeyPressed(DIK_H)) {
            api->ShowBallMessage(api->GetPlayer(),(char*)"Hamsterball 2 Unlocked");
        }

        if (api->IsKeyDown(DIK_S)) {
            Ball* player = api->GetPlayer();
            PhysicsObject* physics = api->GetPhysicsObj();
            if (physics == nullptr) {
                return;
            }
            Vec3 playerPos = Vec3(player->pos_x, player->pos_y, player->pos_z);
            if (api->LevelRaycastHit(playerPos, Vec3(-1, 0, 0), 100)) {
                physics->velocity_x += .5;
            }
            if (api->LevelRaycastHit(playerPos, Vec3(1, 0, 0), 100)) {
                physics->velocity_x -= .5;
            }
            if (api->LevelRaycastHit(playerPos, Vec3(0, 0, 1), 100)) {
                physics->velocity_z -= .5;
            }
            if (api->LevelRaycastHit(playerPos, Vec3(0, 0, -1), 100)) {
                physics->velocity_z += .5;
            }
        }
        if (api->WasKeyPressed(DIK_A)) {
            CallMethod(0x016F0, player, 0.0f, 1.0f, 0.0f, 1000.0f);
        }

        if (api->WasKeyPressed(DIK_C)) {
            PhysicsObject* physics = api->GetPhysicsObj();
            if (physics->gravity_x == 0) {
                physics->gravity_x = -1;
            }
            else if (physics->gravity_x == -1) {
                physics->gravity_x = 1;
            }
            else {
                physics->gravity_x = 0;
            }
        }

        if (api->WasKeyPressed(DIK_Y)) {
            PhysicsObject* physics = api->GetPhysicsObj();
            if (physics->gravity_y == 0) {
                physics->gravity_y = -1;
            }
            else if (physics->gravity_y == -1) {
                physics->gravity_y = 1;
            }
            else {
                physics->gravity_y = 0;
            }
        }

        if (api->WasKeyPressed(DIK_Z)) {
            PhysicsObject* physics = api->GetPhysicsObj();
            if (physics->gravity_z == 0) {
                physics->gravity_z = -1;
            }
            else if (physics->gravity_z == -1) {
                physics->gravity_z = 1;
            }
            else {
                physics->gravity_z = 0;
            }
        }

        if (api->WasKeyPressed(DIK_G)) {
            PhysicsObject* physics = api->GetPhysicsObj();
            if (physics->gravity_y == -1) {
                physics->gravity_y = -0.25f;
            }
            else if (physics->gravity_y == -0.25f) {
                physics->gravity_y = .25f;
            }
            else {
                physics->gravity_y = -1;
            }
        }
        if (api->WasKeyPressed(DIK_V)) {
            Ball* player = api->GetPlayer();
            PhysicsObject* physics = api->GetPhysicsObj();
            Vec3 playerPos = Vec3(player->pos_x, player->pos_y, player->pos_z);
            api->LevelRaycastHit(playerPos, Vec3(1, 0, 0), 1);
        }
        if (api->WasKeyPressed(DIK_B)) {
            Ball* player = api->GetPlayer();
            PhysicsObject* physics = api->GetPhysicsObj();
            Vec3 playerPos = Vec3(player->pos_x, player->pos_y, player->pos_z);
            api->LevelRaycastHit(playerPos, Vec3(0, 1, 0), 1);
        }
        if (api->WasKeyPressed(DIK_N)) {
            Ball* player = api->GetPlayer();
            PhysicsObject* physics = api->GetPhysicsObj();
            Vec3 playerPos = Vec3(player->pos_x, player->pos_y, player->pos_z);
            api->LevelRaycastHit(playerPos, Vec3(0, 0, 1), 1);
        }

        if (api->IsKeyDown(DIK_D)) {
            api->SetSpeed(player, 10.0f);
        }

        if (api->WasKeyPressed(DIK_M)) {
            size_t enemyCount;
            Ball* enemy = api->GetEnemies(&enemyCount)[0]; 
            enemy->radius = 100.0f;
        }

        DWORD baseAddr = api->GetGameBaseAddress();

        if (api->WasKeyPressed(DIK_J)) {
            printf("TEST");
            DWORD vtable = baseAddr + 0xCF300;
            void* font = *(void**)((char*)api->GetApp() + 0x318);
            Color tc = Color(.5f, .5f, .5f, .8f);
            CallMethod(0x2C870, font, "Test 123", 300, 50, vtable, tc.r, tc.g, tc.b, tc.a);
        }

        if (api->WasKeyPressed(DIK_F)) {
            showText = !showText;
            printf("New value: %s\n", (showText) ? "True" : "False");
        }

        if (api->WasKeyPressed(DIK_T)) {
            CustomText text(api->GetApp()->fonts.arialNarrow12bold, 300, y, Color(.35f, .35f, .8f, 1.0f), true);
            api->DrawTimedMessage("TIMED MESSAGE", text, 3);
            y += 30;
            if (y > 500) {
                y = 10;
            }
        }

        if (api->WasKeyPressed(DIK_5)) {
            Ball* p = api->GetPlayer();
            Vec3 pos = Vec3((p->pos_x + 50), (p->pos_y), (p->pos_z + 50));
            Vec3 home_pos = Vec3((p->pos_x + 50), (p->pos_y), (p->pos_z + 50));
            api->CreateBadBall(pos, home_pos);
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
        if (api->WasKeyPressed(DIK_E)) {
            api->ShatterBall(api->GetPlayer());

        }
        if (api->WasKeyPressed(DIK_O)) {
            size_t enemyCount;
            Ball* enemy = api->GetEnemies(&enemyCount)[0];
            api->ShatterBall(enemy);

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

    void onBallBump(Ball* ball1, Ball* ball2) override {
        printf("COLLISION, ball1: %x, ball2: %x, ", ball1, ball2);
        Scene* scene = api->GetScene();
        if (scene != nullptr)
            printf("time: %d\n-----------------------------\n", scene->frame_counter);
    }
};

extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() {
    return new PrintArgs();
}