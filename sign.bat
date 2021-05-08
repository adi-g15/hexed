@REM Execute this command or this script inside the Developer Command Prompt for Visual Studio, or anywhere if signtool is in PATH

@REM This uses the '/a' flag, to automatically chose a certi to sign it. If it's your first time, simply follow these steps to create your own : https://gist.github.com/adi-g15/e2c84795dbfd43b85d1631aa2cac0344
@REM You can raise an issue in the issue section too, if you need help :-)

signtool sign /a x64\Release\hexed.exe
