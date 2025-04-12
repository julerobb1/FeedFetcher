#define UNICODE
#include <windows.h>
#include <CommCtrl.h>
#include <iostream>
#include "app_logic.h" // Include shared declarations
#include <cstdlib>
#include <string>

#pragma comment(lib, "Comctl32.lib")

#ifndef IDC_PROGRESS
#define IDC_PROGRESS 1002
#endif


static HWND hProgressBar;

// Function to check if the system supports graphical UI
bool isGraphicalEnvironment() {
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    GetVersionExW((OSVERSIONINFOW*)&osvi);

    // Check if the OS is a server version (e.g., Windows Server)
    if (osvi.wProductType == VER_NT_SERVER) {
        return false; // Assume no graphical environment for server OS
    }

    // Additional checks for graphical environment can be added here
    return true;
}

// Forward declaration for runFFmpeg
void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile);

void runTextUI(int argc, wchar_t* argv[]) {
    if (argc < 3) {
        std::wcerr << L"Usage: Mp3Combiner.exe <broadcastify_feed_url> <output_file> [file1 file2 ...] [--transcribe]\n";
        return;
    }

    std::wstring feedUrl = argv[1];
    std::wstring outputFile = argv[2];
    std::vector<std::wstring> inputFiles;
    bool transcribe = false;

    for (int i = 3; i < argc; ++i) {
        if (std::wstring(argv[i]) == L"--transcribe") {
            transcribe = true;
        } else {
            inputFiles.push_back(argv[i]);
        }
    }

    // Perform operations
    downloadFeedArchives(NULL, NULL, feedUrl);
    if (!inputFiles.empty()) {
        combineFiles(outputFile, inputFiles);
    }
    if (transcribe) {
        std::wstring wavFile = L"transcribed_" + outputFile + L".wav";
        runFFmpeg(outputFile, wavFile); // Convert to WAV format
        runTranscriber(wavFile);       // Pass the WAV file to the transcriber
    }
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(icex);
            icex.dwICC = ICC_PROGRESS_CLASS;
            InitCommonControlsEx(&icex);

            hProgressBar = CreateWindowExW(
                0, PROGRESS_CLASSW, NULL,
                WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                10, 50, 300, 30,
                hWnd, (HMENU)IDC_PROGRESS,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );
            SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(hProgressBar, PBM_SETPOS, 0, 0);

            CreateWindowW(
                L"BUTTON", L"Start Download",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                10, 10, 120, 30,
                hWnd, (HMENU)1,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            CreateWindowW(
                L"BUTTON", L"Run Transcriber",
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
                loginToBroadcastify();
                downloadFeedArchives(hWnd, hProgressBar, L"https://www.broadcastify.com/archives/feed/39972");
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (isGraphicalEnvironment()) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = MainWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"FeedDownloaderClass";
        RegisterClassW(&wc);

        HWND hWnd = CreateWindowW(
            wc.lpszClassName,
            L"Feed Downloader", // Wide-character string
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            350, 150,
            NULL, NULL,
            hInstance,
            NULL
        );

        if (!hWnd) {
            MessageBoxW(NULL, L"Failed to create the main window.", L"Error", MB_ICONERROR);
            return -1;
        }

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } else {
        while (true) {
            std::wcout << L"Select an action:\n";
            std::wcout << L"1. Download Feed Archives\n";
            std::wcout << L"2. Transcribe Audio\n";
            std::wcout << L"3. Exit\n";
            std::wcout << L"Enter your choice: ";

            int choice;
            std::wcin >> choice;

            if (choice == 1) {
                std::wstring feedUrl;
                std::wcout << L"Enter the Broadcastify feed URL: ";
                std::wcin.ignore(); // Clear newline from input buffer
                std::getline(std::wcin, feedUrl);

                loginToBroadcastify();
                downloadFeedArchives(NULL, NULL, feedUrl);
            } else if (choice == 2) {
                std::wstring inputFile, outputFile;
                std::wcout << L"Enter the input audio file path: ";
                std::wcin.ignore(); // Clear newline from input buffer
                std::getline(std::wcin, inputFile);

                std::wcout << L"Enter the output WAV file path: ";
                std::getline(std::wcin, outputFile);

                runFFmpeg(inputFile, outputFile);
                runTranscriber(outputFile);
            } else if (choice == 3) {
                std::wcout << L"Exiting program.\n";
                break;
            } else {
                std::wcout << L"Invalid choice. Please try again.\n";
            }
        }
    }

    return 0;
}

void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile) {
    std::wstring ffmpegPath = L"Z:\\Github\\FeedFetcher\\bin\\ffmpeg.exe";
    std::wstring command = L"\"" + ffmpegPath + L"\" -i \"" + inputFile + L"\" \"" + outputFile + L"\"";
    int result = _wsystem(command.c_str());
    if (result != 0) {
        std::wcerr << L"FFmpeg command failed with error code: " << result << std::endl;
    }
}

void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& feedUrl) {
    std::wstring curlPath = L"Z:\\Github\\FeedFetcher\\bin\\curl.exe";
    std::wstring outputFile = L"feed_archive.zip";
    std::wstring command = L"\"" + curlPath + L"\" -L -o \"" + outputFile + L"\" \"" + feedUrl + L"\"";

    int result = _wsystem(command.c_str());
    if (result != 0) {
        MessageBoxW(hWnd, L"Failed to download feed archives.", L"Error", MB_ICONERROR);
    } else {
        MessageBoxW(hWnd, L"Feed archives downloaded successfully.", L"Info", MB_ICONINFORMATION);
    }
}

void loginToBroadcastify() {
    std::wstring username, password;

    // Prompt user for username
    std::wcout << L"Enter your Broadcastify username: ";
    std::getline(std::wcin, username);

    // Prompt user for password
    std::wcout << L"Enter your Broadcastify password: ";
    std::getline(std::wcin, password);

    std::wstring curlPath = L"Z:\\Github\\FeedFetcher\\bin\\curl.exe";
    std::wstring loginUrl = L"https://www.broadcastify.com/login/";
    std::wstring command = L"\"" + curlPath + L"\" -L -d \"username=" + username + L"&password=" + password + L"\" \"" + loginUrl + L"\"";

    int result = _wsystem(command.c_str());
    if (result != 0) {
        MessageBoxW(NULL, L"Failed to log in to Broadcastify.", L"Error", MB_ICONERROR);
    } else {
        MessageBoxW(NULL, L"Login successful.", L"Info", MB_ICONINFORMATION);
    }
}
