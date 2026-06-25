# Hamsterball Plus Modding API

This is a modding standard for the retro video game _Hamsterball_. Using DLL proxying and MinHook, this project creates a modding standard for the community; allowing players to easily install mods, while allowing modders to more easily create mods that can coexist.

## How to install

1. Download the hbPlusMod.dll, ModConfig.ini, and Mods folder from releases.
2. Drop all three into your Hamsterball folder. (Your path is likely something like this: C:\Program Files (x86)\Raptisoft\Hamsterball\\)
3. Right click on the Hamsterball.exe and go to properties.
4. On the compatability tab, check off "Run this program as an administrator". (This is needed in order to modify the .ini file as well as to run custom maps)
5. That's it! From here, you can simply place mods within the \Mods folder. To uninstall mods just delete the .dll file from the \Mods folder.

## Editing Custom Controls

You can edit custom modded controls by editing the ModConfig.ini file. Under the [Custom Controls] section, you will see the IDs and corresponding keys of the controls (ex. jump=0x2A, RELOAD_INI_ML=CTRL+0x13). The hex show on the right corresponds to DirectInput 8 keyboard scan codes. To change a keybind, just swap out that number for valid scan code. You can find the codes
here: [Keycodes](https://gist.github.com/tracend/912308). If you want the control to be a ctrl command (like you have to press ctrl+r instead of just r), then just add "ctrl+" to the beginning of the code. Currently this system is not very user friendly, but this will eventually have a proper user interface when the eventual mod loader is made. You'll also see the [Unused Controls] section. This is for controls from mods that are no longer installed. This saves the old controls in case you decide to reinstall the mod. You are free to delete these if you want. Additionally, there are also the settings for the custom options in the .ini file. You can change these but be careful with how you do it.

> **This is the end of the guide for players. If you are interested in creating mods, then continue on with the rest of the tutorial.**

# Modders Guide

## ModConfig.ini

**[Config] Section**  
`ShowConsole`: 1 = On, 0 = False. This is really useful for debugging. There are some error messages that the modding API will automatically show, but using printf() for testing is essential.

**[Theme] Section**  
Leave this section blank if you want to use the game defaults.  
`MenuBodyR`, `MenuBodyG`, `MenuBodyB`, `MenuBodyA` Control the color of the options and time trial menu using float values from 0.0-1.0  
`MenuHeaderR`, `MenuHeaderG`, `MenuHeaderB`, `MenuHeaderA` Control the color of the the header to many menus using float values from 0.0-1.0

**[#] (Level) Section**  
Leave this section blank if you want to use the game defaults.
The number in the section corresponds to the level, [0] is warmup race, [1] is beginner race, etc.  
`RaceName` The name of the race  
`ArenaName` The name of the corresponding arena  
`ColorR`, `ColorG`, `ColorB` The color of the level's text in the time trial menus.
`BlotR`, `BlotG`, `BlotB` The color of the the time blot (the shape around the timer) for this specific level

**[Custom Controls] and [Unused Controls] Section**  
This stores the keybind for all of the custom controls that mods use. The entries are created and handled by the modding API itself, so you will not ever need to create new entries. As shown in the "Editing Custom Controls" section, you can change the keybinds. If the keybind has "ctrl+" before the hex code, then the keybind requires ctrl to also be pressed. The unused controls section is used for storing controls for mods that are not currently loaded in. The idea here was that if a user temporarily uninstalled a mod, it would be pretty annoying if they lost their keybinds when they reinstalled it. The controls will automatically be moved to their correct section.

**[Toggle Buttons] and [Unused Toggle Buttons] Section**  
These entries store the values for the custom toggleable options. 1=on, 0=off. Just like with the controls sections, these entries will be saved in their unused section when not in use.

**[Sliders] and [Unused Sliders] Section**  
These entries store the float values of the custom sliders. Just like the controls and toggle button sections, these entries are dynamically stored in their used/unused sections.

## How To Make Mods

1. Download the HBmodTemplate.zip file, and place it in your Visual Studio template folder. The path of that is typically `C:\Users\[YourName]\Documents\Visual Studio\Templates\`. DO NOT EXTRACT THE ZIP FILE
2. Create a Visual Studio project using that template.
3. Rename the fields that are labeled for renaming in MainModFile.cpp. This includes the class name (which needs to be renamed at the top and bottom of the file), the string within GetModName(), and the string within GetAuthorName(). Additionally, you can rename MainModFile.cpp but that is optional.
4. This project will be for a single DLL mod that will eventually go in the \Mods folder. Use the functions and structs from HamsterballAPI.h. The following sections go over the major topics, but there are many topics that are not covered here, and are instead covered by the comments for each function/struct/field.

### Base files overview

To create mods, there are two main files you will use: HamsterballAPI.h and MainModFile.cpp. MainModFile.cpp is the file where you will write your code, you should NEVER change anything in HamsterballAPI.h. Additionally, ensure that your version of HamsterballAPI.h is the most up to date version that has been released (Should be version 1). MainModFile.cpp is the main file for the mod, however you can still create more files for helper functions, etc. You should not remove any of the original code from this file, you should just build off of it. I will now go each relevant thing within the base MainModFile.cpp

#### MainModFile.cpp

- `const char* GetModName() override { return "PUT YOUR MOD NAME HERE"; }` This is a required function which returns the name of your mod. Make sure to rename the string in the return.
- `const char* GetAuthorName() override { return "PUT YOUR NAME HERE"; }` Similarly, this is a required function which returns your name. Make sure to rename it. In the header file, you will notice a function called GetContributors(). Add this one as well if there are any other people who helped you with the mod, but that function is not required to compile.
- `int GetApiVersion() override { return HAMSTERBALL_API_VERSION; }` DO NOT EDIT THIS. Future versions of mod (the bass.dll) will not be compatible with DLLs that were originally compiled with an older version. (They will need to be recompiled with the new HamsterballAPI.h). This function helps with tracking the versions that a DLL was built with.
- `void Initialize(IModAPI* modApi) override {}` This is the function that will run when the mod is initially loaded. Put the code that you want to run at the start here.
- `private: IModAPI* api = nullptr;` and `api = modApi;` This initalizes the `api` field, which is an instance of the IModAPI class, which is how you will do most interactions with the game.
- `extern "C" __declspec(dllexport) HamsterballAPI* CreateModInstance() { return new REPLACE_WITH_YOUR_MOD_NAME();}` This is some boiler plate code related to loading DLL files that you need to keep here. Don't worry about it, just leave it untouched at the bottom of the file.

#### HamsterballAPI.h

- All of functions and structs are commented, so you can find more information about them there.
- You will use IModAPI (the api field) to directly interact with the game by calling the functions within. For the functions within the HamsterballAPI class, you will override them instead of calling them. In general, the HamsterballAPI functions are for doing something when something happens in the game, IModAPI functions are more for retrieving values from the game or doing actions in game.
- The file also contains structs. There are two main kinds: ones for calling functions, and ones that map to objects from the game.
- The Color, CustomButton, CustomSlider, CustomText, and CustomControl structs are used for calling the functions. I went with this approach as many of the fields have default values, so this is generally easier for the modder.
- The PhysicsConstants, Ball, Scene, PhysicsObject, Sounds, Fonts, and App structs map to game objects. Some of the fields are commented, but many of them are self explanatory. You will notice that many of these structs have padding fields. This is just to align the memory correctly, you shouldn't use these fields. With that being said, there are likely some fields that are missing, that are stored somewhere in the padding. If you find some that you can verify, please let me know and I will add them to a future version of the API. Many of these fields are also labeled as unverified. These are ones that I'm not 100% sure on, especially the ones that map to some other class that I have not mapped out at all. It's also worth noting that some of these fields can be read from but not written to. I've tried to comment those, but there are likely some that were missed.
- The game has a global App object that is used for a lot of the settings and unlocks. You can get a pointer to this object with api->GetApp()
- The Ball object is used for the players, as well as Badballs (and that one enemy ball on sky race). The playerID field determines which player/if it's an enemy (-1: enemy, 0: player 1, 1: player 2, 2: player3, 3:player4). Each ball has its own PhysicsObject inside which has many useful fields such as the velocity and gravity.
- The Scene object represents the current level in play. This has many fields such as camera options, scores, the current balls in the level, etc.
- The PhysicsConstants is largely unmapped, but it contains some friction settings and other miscellaneous global settings. It's worth noting that I had to use VirtualProtect in order to unlock this memory for writing (even though this wasn't a problem with Cheat Engine), so it's possible the same will have to be done with other fields in other structs.

### Creating and using custom controls

The game handles inputs through DirectInput 8 keycodes. Instead of hardcoding the keycode for hotkeys, I've created a system for custom controls which can be rebinded. You create a control with its own ID and a default value, and then it can be rebound through the ModConfig.ini file. To create these, use u`RegisterCustomControl()` within the Initialize() function in your mod. For the ID, make sure to choose something that clearly tells the user what it does, while also making it unique enough that other mods are unlikely to use it. If another mod uses the same ID then there can be conflicts, so choose carefully. For setting the default keycode, you can use the constants such as DIK_R, DIK_LSHIFT, etc., or the hex values from [here](https://gist.github.com/tracend/912308).

###
