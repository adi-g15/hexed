@REM Execute this command or this script inside the Developer Command Prompt for Visual Studio, or anywhere if msbuild and cl are available in the shell/command prompt

@REM -m is for parallel
@REM -p passing Configuration and Platform params
msbuild hexed.sln -m -p:Configuration=Release -p:Platform=x64
