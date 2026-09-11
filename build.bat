@echo off
cd /d "%~dp0"

cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Dev/llvm-22"
cmake --build . --config Debug --parallel 12
goto :eof

:error
echo [ERROR] An error occurred.
pause
exit /b 1