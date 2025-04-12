#include "app_logic.h"
#include <windows.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <CommCtrl.h>

// Helper function to convert std::wstring to LPCSTR
std::string WStringToString(const std::wstring& wstr) {
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], sizeNeeded, NULL, NULL);
    return str;
}

// Helper function to execute a system command
bool executeCommand(const std::wstring& command) {
    int result = _wsystem(command.c_str());
    return result == 0;
}

void loginToBroadcastify(const std::string& username, const std::string& password) {
    std::string curlPath = "c:/Users/juler/Downloads/curl-8.12.1_4-win64-mingw/curl-8.12.1_4-win64-mingw/bin/curl.exe";

    std::string cookieFile = "cookies.txt";
    std::string loginUrl = "https://www.broadcastify.com/login";
    std::string command = curlPath + " -c " + cookieFile + " -d \"username=" + username + "&password=" + password + "\" " + loginUrl;

    int result = std::system(command.c_str());
    if (result == 0) {
        MessageBoxW(NULL, L"Login successful.", L"Info", MB_OK);
    } else {
        MessageBoxW(NULL, L"Failed to log in. Check your credentials.", L"Error", MB_ICONERROR);
    }
}

void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& feedUrl) {
    std::wstring curlPath = L"c:/Users/juler/Downloads/curl-8.12.1_4-win64-mingw/curl-8.12.1_4-win64-mingw/bin/curl.exe";

    std::wstring outputFile = L"archives/feed_archive.zip";
    std::wstring command = curlPath + L" -o " + outputFile + L" " + feedUrl;

    SendMessage(hProgressBar, PBM_SETPOS, 50, 0); // Update progress bar to 50%

    int result = _wsystem(command.c_str());
    if (result == 0) {
        SendMessage(hProgressBar, PBM_SETPOS, 100, 0); // Update progress bar to 100%
        MessageBoxW(hWnd, L"Feed archives downloaded successfully.", L"Success", MB_OK);
    } else {
        MessageBoxW(hWnd, L"Failed to download feed archives.", L"Error", MB_ICONERROR);
    }
}

void runTranscriber(const std::wstring& audioFilePath) {
    std::wstring command = L"transcriber_tool.exe " + audioFilePath + L" > transcription.txt";
    if (executeCommand(command)) {
        MessageBoxW(NULL, L"Transcription completed successfully. Output saved to transcription.txt.", L"Info", MB_OK);
    } else {
        MessageBoxW(NULL, L"Failed to transcribe the audio file.", L"Error", MB_ICONERROR);
    }
}

void trimSilence(const std::wstring& inputFile, const std::wstring& outputFile) {
    std::wstring command = L"ffmpeg -i " + inputFile + L" -af silenceremove=1:0:-50dB " + outputFile;
    if (executeCommand(command)) {
        MessageBoxW(NULL, L"Silence trimmed successfully.", L"Info", MB_OK);
    } else {
        MessageBoxW(NULL, L"Failed to trim silence from the audio file.", L"Error", MB_ICONERROR);
    }
}

void analyzeAudio(const std::wstring& inputFile) {
    // Placeholder for future implementation
    MessageBoxW(NULL, L"Audio analysis is not yet implemented.", L"Info", MB_OK);
}

void combineFiles(const std::wstring& outputFile, const std::vector<std::wstring>& inputFiles) {
    std::wstring tempFileList = L"file_list.txt";
    FILE* fileList = _wfopen(tempFileList.c_str(), L"w");
    if (!fileList) {
        MessageBoxW(NULL, L"Failed to create temporary file list.", L"Error", MB_ICONERROR);
        return;
    }

    for (const auto& inputFile : inputFiles) {
        fwprintf(fileList, L"file '%s'\n", inputFile.c_str());
    }
    fclose(fileList);

    std::wstring command = L"ffmpeg -f concat -safe 0 -i " + tempFileList + L" -c copy " + outputFile;
    if (executeCommand(command)) {
        MessageBoxW(NULL, L"Files combined successfully.", L"Info", MB_OK);
    } else {
        MessageBoxW(NULL, L"Failed to combine files.", L"Error", MB_ICONERROR);
    }

    _wremove(tempFileList.c_str()); // Clean up temporary file
}
