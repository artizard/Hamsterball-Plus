#include "pch.h"
#include "HamsterballAPI.h"
#include <windows.h>
#include "CoreFeatures.h"
/// @brief This contains the buttons and such that I want to be built in with the mod loader. I made this the same way as a mod for simplicity. 
IModAPI* api = nullptr;
const char* CoreFeatures::GetModName() { return "Core Features"; }
const char* CoreFeatures::GetAuthorName() { return "arti"; }
int CoreFeatures::GetApiVersion() { return HAMSTERBALL_API_VERSION; }

void CoreFeatures::Initialize(IModAPI* modApi) {
    api = modApi;

    api->RegisterCustomControl("RESPAWN_PLAYER_ML", DIK_X);

    CustomButton respawnButton("RESPAWN_PLAYER_STATE_ML", "RESPAWN BUTTON");
    respawnButton.trueText = "ON";
    respawnButton.falseText = "OFF";
    api->CreateToggleButton(respawnButton, this);
}

void CoreFeatures::onPlayerUpdate(Ball* playerObject) {
    if (playerObject->playerID != 0) return; // only affect player 1
    if (api->GetButtonState("RESPAWN_PLAYER_STATE_ML")) { // if cheat_jump is enabled
        if (api->WasControlPressed("RESPAWN_PLAYER_ML")) { // if the jump control was pressed 
            api->RespawnPlayer(); 
        }
    }
}