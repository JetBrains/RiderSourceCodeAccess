How to add RiderSourceCodeAccess to your project
### 
1. Copy `RiderSourceCodeAccess` folder:
   1. Engine from sources: to `{UnrealEngineRoot}/Engine/Plugins/Developer` or to `{UnrealProjectRoot}/Plugins/Developer`;
   2. Engine from Epic store: to `{UnrealProjectRoot}/Plugins/Developer`;
2. Re-generate solution files - uproject file > context menu > Generate Visual Studio project files;
3. Build UE project using Rider or Visual Studio;
4. Start Unreal Editor;
5. Edit > Editor Preferences ... > General > Source Code > Source Code Editor;
6. Select Rider from drop down list. NB: only Rider with C++ plugins will be available from drop down list.