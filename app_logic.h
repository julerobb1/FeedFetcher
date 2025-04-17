#ifndef APP_LOGIC_H
#define APP_LOGIC_H

#include <string>
#include <vector>
#include <windows.h>

// Declare shared functions
std::string WStringToString(const std::wstring& wstr);
void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& feedUrl);
void runTranscriber(const std::wstring& audioFilePath);
void combineFiles(const std::wstring& outputFile, const std::vector<std::wstring>& inputFiles);
void trimSilence(const std::wstring& inputFile, const std::wstring& outputFile); // Updated for ffmpeg
void analyzeAudio(const std::wstring& inputFile); // Placeholder for future analysis

#endif // APP_LOGIC_H
