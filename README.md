# Description
RiderSourceCodeAccess is a plugin for Unreal Engine, available through [Marketplace page](https://www.unrealengine.com/marketplace/en-US/product/rider-source-code-access) that will add an option to select ["Rider for Unreal Engine"](https://www.jetbrains.com/rider/unreal/) as your source code editor in Unreal Editor.

## Functionality
* Adds the option to select "Rider for Unreal Engine" as your IDE of choice in Unreal Editor.
* You can select a specific version of Rider (e.g. Rider xxx.xx.xx) or,
* Use the latest version of Rider that’s available on your workstation.
![Example of dropdown box with Rider for Unreal Engine](https://cdn1.epicgames.com/ue/product/Screenshot/RiderSourceCodeAccesss-1920x1080-32507ce3de2846e20de0ef72904f707a.png)

# How to add this plugin manually (not recommened)
## How to add to Game project
1. Go to ["Releases" page](https://github.com/JetBrains/RiderSourceCodeAccess/releases) and download version of plugin for your version of Unreal Engine; 
2. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
3. Copy `RiderSourceCodeAccess` folder to `{GameProjectRoot}/Plugins/Developer`;
  a. If `{GameProjectRoot}/Plugins/Developer` does not exist, create it;
4. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
5. Build UE project using Rider or Visual Studio;
6. Start Unreal Editor;
7. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
8. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.

## How to add to Engine project
### Unreal Engine is built from source code   
1. Go to ["Releases" page](https://github.com/JetBrains/RiderSourceCodeAccess/releases) and download version of plugin for your version of Unreal Engine; 
2. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
3. Copy `RiderSourceCodeAccess` folder to `{UnrealEngineRoot}/Engine/Plugins/Developer`;
  a. If `{UnrealProjectRoot}/Plugins/Developer` does not exist, create it;
4. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
5. Build UE project using Rider or Visual Studio;
6. Start Unreal Editor;
7. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
8. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.

### Unreal Engine is downloaded from Epic Games Store
1. Go to ["Releases" page](https://github.com/JetBrains/RiderSourceCodeAccess/releases) and download version of plugin for your version of Unreal Engine; 
2. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
3. Copy `RiderSourceCodeAccess` folder to `{UnrealProjectRoot}/Plugins/Developer`;
  a. If `{UnrealProjectRoot}/Plugins/Developer` does not exist, create it;
4. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
5. Build UE project using Rider or Visual Studio;
6. Move `RiderSourceCodeAccess` folder from `{UnrealProjectRoot}/Plugins/Developer` to `{UnrealEngineRoot}/Engine/Plugins/Developer`;
7. Start Unreal Editor;
8. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
9. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.
