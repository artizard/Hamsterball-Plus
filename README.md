# Hamsterball DLL Mod

This is a mod which currently does the following things:
- Press X to die instantly
- Custom option in the options menu (currently does nothing)
- Add widescreen resolutions to resolution selector
- Custom level name and text color support (defined in CustomLevels.ini)
- Custom menu color support (CustomLevels.ini)

## How it works

This is made using a technique called "DLL Injection". By naming this custom DLL "bass.dll" and renaming the original bass.dll to "bass_real.dll", the game will load and run our code when it is open, and our code can change and add anything in the game. (When the game calls the original bass.dll functions, our code will pass them along to the original dll). Function hooking is done using the MinHook library, so that our code can easily do its own logic when any game function is run.
