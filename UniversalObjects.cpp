#include "pch.h"
#include "UniversalObjects.h"
#include "GameEngine.h"
#include <unordered_map>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// This file has implementations of all level specific objects, so that way they can work in all levels!

struct BumperCollisionData {
    BYTE pad[0x418];
};

#define MAX_CUSTOM_BUMPERS 20
BumperCollisionData g_CustomBumpers[MAX_CUSTOM_BUMPERS];

std::unordered_map<int, DWORD> g_BumperCooldowns;

//typedef void(__fastcall* tPlayBumperEffect)(void* ecx_effectManager, void* edx_dummy, float posX, float posY, float posZ);

void __fastcall Hooked_MasterLevelSetup(int* param_1) {
    Original_MasterLevelSetup(param_1);


    // Turns out this isn't actually needed for bumpers to work. Uncomment if needed later
//    // Grab the specific Level Name string at offset 0x868
//    char* boardName = *(char**)((DWORD)param_1 + 0x868);
//
//    // Only awaken custom DLL bumpers if we are NOT in Beginner Race
//    if (boardName != nullptr && _stricmp(boardName, "Board (Beginner)") != 0) {
//
//        void* meshWorldPtr = (void*)param_1[0x22b];
//
//        if (meshWorldPtr != nullptr) {
//            for (int i = 0; i < MAX_CUSTOM_BUMPERS; i++) {
//                char bumperName[32];
//                sprintf_s(bumperName, "N:BUMPER%d", i + 1);
//                Original_BindGeometry(meshWorldPtr, bumperName, &g_CustomBumpers[i]);
//            }
//        }
//    }
}

void __fastcall Hooked_BaseCollideCheck(void* this_ptr, void* edx, int* ballPhysics, int* collisionData) {

    if (collisionData != nullptr && collisionData[1] != 0) {
        char* hitNodeName = *(char**)(collisionData[1] + 0x864);

        if (hitNodeName != nullptr && _strnicmp(hitNodeName, "N:BUMPER", 8) == 0) {

            char* boardName = *(char**)((DWORD)this_ptr + 0x868);

            if (boardName != nullptr && _stricmp(boardName, "Board (Beginner)") != 0) {

                int bumperId = atoi(hitNodeName + 8);
                DWORD currentTime = GetTickCount();

                if (g_BumperCooldowns.find(bumperId) == g_BumperCooldowns.end() ||
                    (currentTime - g_BumperCooldowns[bumperId]) > 250) {

                    g_BumperCooldowns[bumperId] = currentTime;

                    // TODO: Call the  game's PlaySound3D function to play the sound
                    // TODO: Also fix the white flash not showing

                    DWORD rigidBodyPtr = *(DWORD*)((DWORD)ballPhysics + 0x1a4);

                    if (rigidBodyPtr != 0) {
                        float* velX = (float*)(rigidBodyPtr + 0xca4);
                        float* velZ = (float*)(rigidBodyPtr + 0xcac);

                        *velX *= 4.0f;
                        *velZ *= 4.0f;

                        float speed = sqrtf((*velX * *velX) + (*velZ * *velZ));

                        if (speed > 0.0f) {
                            if (speed < 5.0f) {
                                *velX = (*velX / speed) * 5.0f;
                                *velZ = (*velZ / speed) * 5.0f;
                            }
                            else if (speed > 10.0f) {
                                *velX = (*velX / speed) * 10.0f;
                                *velZ = (*velZ / speed) * 10.0f;
                            }
                        }
                    }
                }
            }
        }
    }

    Original_BaseCollideCheck(this_ptr, edx, ballPhysics, collisionData);
}