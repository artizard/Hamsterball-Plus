# Hamsterball DLL Mod

This is a mod which currently does the following things:
- Press X to die instantly
- Cheats (Uncap speed, no break, jump button), toggled in Options Menu
- Add widescreen resolutions to resolution selector
- Custom level name and text color support (defined in CustomLevels.ini)
- Custom menu color support (CustomLevels.ini)

## INSTALLATION INSTRUCTIONS

1. Download the files for the latest version in the Releases section on the right
2. Go in your Hamsterball installation folder and rename "bass.dll" to "bass_real.dll"
3. Place the "hb-mod.dll" file in your installation folder and rename it to "bass.dll"
4. Place the "ModConfig.ini" file in your installation folder. You can open it in a text editor to change it however you want.

## How it works

This is made using a technique called "DLL Injection". By naming this custom DLL "bass.dll" and renaming the original bass.dll to "bass_real.dll", the game will load and run our code when it is open, and our code can change and add anything in the game. (When the game calls the original bass.dll functions, our code will pass them along to the original dll). Function hooking is done using the MinHook library, so that our code can easily do its own logic when any game function is run.
