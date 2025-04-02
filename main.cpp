#include <windows.h>
#include <CommCtrl.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>

#pragma comment(lib, "Comctl32.lib")

#ifndef IDC_PROGRESS
#define IDC_PROGRESS 1002
#endif

static HWND hProgressBar;

void loginToBroadcastify(const std::string& username, const std::string& password) {
    std::string curlPath = "third_party/curl/x64/curl.exe"; // Hardcoded to 64-bit curl

    std::string cookieFile = "cookies.txt";
    std::string loginUrl = "https://www.broadcastify.com/login";
    std::string command = curlPath + " -c " + cookieFile + " -d \"username=" + username + "&password=" + password + "\" " + loginUrl;

    int result = std::system(command.c_str());
    if (result == 0) {
        MessageBoxA(NULL, "Login successful.", "Info", MB_OK);
    } else {
        MessageBoxA(NULL, "Failed to log in. Check your credentials.", "Error", MB_ICONERROR);
    }
}

void downloadFeedArchives(HWND hWnd) {
    std::string curlPath = "third_party/curl/x64/curl.exe"; // Hardcoded to 64-bit curl

    std::string feedUrl = "https://example.com/user/feeds/today";
    std::string outputFile = "archives/feed_archive.zip";
    std::string command = curlPath + " -o " + outputFile + " " + feedUrl;

    SendMessage(hProgressBar, PBM_SETPOS, 50, 0); // Update progress bar to 50%

    int result = std::system(command.c_str());
    if (result == 0) {
        SendMessage(hProgressBar, PBM_SETPOS, 100, 0); // Update progress bar to 100%
        MessageBoxA(hWnd, "Feed archives downloaded successfully.", "Success", MB_OK);
    } else {
        MessageBoxA(hWnd, "Failed to download feed archives.", "Error", MB_ICONERROR);
    }
}

void runTranscriber(const std::string& audioFilePath) {
    std::string command = "run_transcriber.bat " + audioFilePath;
    int result = std::system(command.c_str());
    if (result == 0) {
        MessageBoxA(NULL, "Transcription completed successfully.", "Info", MB_OK);
    } else {
        MessageBoxA(NULL, "Failed to transcribe the audio file.", "Error", MB_ICONERROR);
    }
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(icex);
            icex.dwICC = ICC_PROGRESS_CLASS;
            InitCommonControlsEx(&icex);

            hProgressBar = CreateWindowEx(
                0, PROGRESS_CLASS, NULL,
                WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                10, 50, 300, 30,
                hWnd, (HMENU)IDC_PROGRESS,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );
            SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(hProgressBar, PBM_SETPOS, 0, 0);

            CreateWindow(
                TEXT("BUTTON"), TEXT("Start Download"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                10, 10, 120, 30,
                hWnd, (HMENU)1,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            CreateWindow(
                TEXT("BUTTON"), TEXT("Run Transcriber"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                140, 10, 120, 30,
                hWnd, (HMENU)2,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) { // Start Download button clicked
                std::string username = "your_username"; // Replace with actual username
                std::string password = "your_password"; // Replace with actual password
                loginToBroadcastify(username, password);
                downloadFeedArchives(hWnd);
            } else if (LOWORD(wParam) == 2) { // Run Transcriber button clicked
                std::string audioFilePath = "path_to_audio_file.wav"; // Replace with actual file path
                runTranscriber(audioFilePath);
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"FeedDownloaderClass";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        wc.lpszClassName,
        L"Feed Downloader",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        350, 150,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hWnd) {
        MessageBox(NULL, L"Failed to create the main window.", L"Error", MB_ICONERROR);
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
