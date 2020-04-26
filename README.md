How to add RiderSourceCodeAccess to your project

## How to add to Game project
1. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
2. Copy `RiderSourceCodeAccess` folder to `{UnrealProjectRoot}/Plugins/Developer`;
3. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
4. Build UE project using Rider or Visual Studio;
5. Start Unreal Editor;
6. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
7. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.

## Hot to add to Engine project
### Project is built from source code   
1. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
2. Copy `RiderSourceCodeAccess` folder to `{UnrealEngineRoot}/Engine/Plugins/Developer`;
3. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
4. Build UE project using Rider or Visual Studio;
5. Start Unreal Editor;
6. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
7. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.

### Project is downloaded from Epic Games Store
1. Unzip `RiderSourceCodeAccess.zip` to `RiderSourceCodeAccess` folder;
2. Copy `RiderSourceCodeAccess` folder to `{UnrealProjectRoot}/Plugins/Developer`;
3. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
4. Build UE project using Rider or Visual Studio;
5. Move `RiderSourceCodeAccess` folder from `{UnrealProjectRoot}/Plugins/Developer` to `{UnrealEngineRoot}/Engine/Plugins/Developer`;
5. Start Unreal Editor;
6. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
7. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.
