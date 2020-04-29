How to add RiderSourceCodeAccess to your project

## How to add to Game project
1. Go to ["Releases" page](https://github.com/JetBrains/RiderSourceCodeAccess/releases) and download version of plugin for your version of Unreal Engine; 
2. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
3. Copy `RiderSourceCodeAccess` folder to `{UnrealProjectRoot}/Plugins/Developer`;
  a. If `{UnrealProjectRoot}/Plugins/Developer` does not exist, create it;
4. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
5. Build UE project using Rider or Visual Studio;
6. Start Unreal Editor;
7. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
8. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.

## How to add to Engine project
### Project is built from source code   
1. Go to ["Releases" page](https://github.com/JetBrains/RiderSourceCodeAccess/releases) and download version of plugin for your version of Unreal Engine; 
2. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
3. Copy `RiderSourceCodeAccess` folder to `{UnrealEngineRoot}/Engine/Plugins/Developer`;
  a. If `{UnrealProjectRoot}/Plugins/Developer` does not exist, create it;
4. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
5. Build UE project using Rider or Visual Studio;
6. Start Unreal Editor;
7. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
8. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.

### Project is downloaded from Epic Games Store
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
