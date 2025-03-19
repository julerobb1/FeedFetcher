@echo off
setlocal

:: Compile the AudioAnalyzer.cpp file
cl /EHsc /FeAudioAnalyzer.exe AudioAnalyzer.cpp

echo Build completed successfully.
endlocal
