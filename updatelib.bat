@echo off
set REPO_URL=https://github.com/matth-x/MicroOcpp.git
set TEMP_DIR=temp-MicroOcpp
set DEST_DIR=D:\PlatformIO\OCPP\.pio\libdeps\esp32dev\MicroOcpp\src

if not exist "%TEMP_DIR%" (
    git clone %REPO_URL% %TEMP_DIR%
) else (
    cd %TEMP_DIR%
    git pull
    cd ..
)

xcopy /E /Y /I "%TEMP_DIR%\src" "%DEST_DIR%"

rd /S /Q "%TEMP_DIR%"

echo 更新完成
pause
