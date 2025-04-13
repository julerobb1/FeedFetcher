#define UNICODE
#include <windows.h>
#include <CommCtrl.h>
#include <iostream>
#include "app_logic.h" // Include shared declarations
#include <cstdlib>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <set>
#include <shlwapi.h> // For PathCombine
#pragma comment(lib, "Shlwapi.lib")

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
    static HWND hBaseUrlEdit, hDateList, hDownloadButton, hTranscribeButton;

    switch (msg) {
        case WM_CREATE: {
            // Create a label for the base URL
            CreateWindowW(
                L"STATIC", L"Base URL:",
                WS_VISIBLE | WS_CHILD,
                10, 10, 80, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create an edit box for the base URL
            hBaseUrlEdit = CreateWindowW(
                L"EDIT", L"https://www.broadcastify.com/archives/feed/",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                100, 10, 300, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create a list box for available dates
            hDateList = CreateWindowW(
                L"LISTBOX", NULL,
                WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                10, 40, 390, 100,
                hWnd, (HMENU)IDC_PROGRESS,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create a button to fetch available dates
            CreateWindowW(
                L"BUTTON", L"Fetch Dates",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                10, 150, 120, 30,
                hWnd, (HMENU)1,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create a button to download selected dates
            hDownloadButton = CreateWindowW(
                L"BUTTON", L"Download",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                140, 150, 120, 30,
                hWnd, (HMENU)2,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create a button to transcribe audio
            hTranscribeButton = CreateWindowW(
                L"BUTTON", L"Transcribe",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                270, 150, 120, 30,
                hWnd, (HMENU)3,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) { // Fetch Dates button clicked
                wchar_t baseUrl[512];
                GetWindowTextW(hBaseUrlEdit, baseUrl, 512);

                std::vector<std::wstring> dates = crawlArchiveDates(baseUrl);
                SendMessageW(hDateList, LB_RESETCONTENT, 0, 0); // Clear the list box

                for (const auto& date : dates) {
                    SendMessageW(hDateList, LB_ADDSTRING, 0, (LPARAM)date.c_str());
                }
            } else if (LOWORD(wParam) == 2) { // Download button clicked
                int count = SendMessageW(hDateList, LB_GETSELCOUNT, 0, 0);
                if (count > 0) {
                    std::vector<int> selectedIndices(count);
                    SendMessageW(hDateList, LB_GETSELITEMS, count, (LPARAM)selectedIndices.data());

                    wchar_t baseUrl[512];
                    GetWindowTextW(hBaseUrlEdit, baseUrl, 512);

                    for (int index : selectedIndices) {
                        wchar_t date[16];
                        SendMessageW(hDateList, LB_GETTEXT, index, (LPARAM)date);

                        std::wstring archiveUrl = std::wstring(baseUrl) + L"/" + date + L"/";
                        downloadFeedArchives(hWnd, hProgressBar, archiveUrl);
                    }
                }
            } else if (LOWORD(wParam) == 3) { // Transcribe button clicked
                MessageBoxW(hWnd, L"Transcription feature is not yet implemented.", L"Info", MB_ICONINFORMATION);
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
    WNDCLASSW wc = {};
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"FeedDownloaderClass";
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        wc.lpszClassName,
        L"Feed Downloader",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        420, 230,
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

    // Get the directory of the executable
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);

    // Construct the path to curl.exe
    wchar_t curlPath[MAX_PATH];
    PathCombineW(curlPath, exePath, L"bin\\curl.exe");

    std::wstring loginUrl = L"https://www.broadcastify.com/login/";
    std::wstring command = L"\"" + std::wstring(curlPath) + L"\" -L -d \"username=" + username + L"&password=" + password + L"\" \"" + loginUrl + L"\"";

    // Run curl and capture output
    FILE* pipe = _wpopen(command.c_str(), L"r");
    if (!pipe) {
        MessageBoxW(NULL, L"Failed to execute curl command.", L"Error", MB_ICONERROR);
        return;
    }

    std::wstring response;
    wchar_t buffer[128];
    while (fgetws(buffer, 128, pipe)) {
        response += buffer;
    }
    _pclose(pipe);

    // Check if login was successful
    if (response.find(L"Login successful") != std::wstring::npos || response.find(L"Welcome") != std::wstring::npos) {
        MessageBoxW(NULL, L"Login successful.", L"Info", MB_ICONINFORMATION);
    } else {
        MessageBoxW(NULL, L"Login failed. Please check your credentials.", L"Error", MB_ICONERROR);
    }
}

std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl) {
    std::wstring curlPath = L"Z:\\Github\\FeedFetcher\\bin\\curl.exe";
    std::wstring command = L"\"" + curlPath + L"\" -L \"" + baseUrl + L"\"";
    std::wstring output;

    // Run curl and capture output
    FILE* pipe = _wpopen(command.c_str(), L"r");
    if (!pipe) {
        std::wcerr << L"Failed to run curl command.\n";
        return {};
    }

    wchar_t buffer[128];
    while (fgetws(buffer, 128, pipe)) {
        output += buffer;
    }
    _pclose(pipe);

    // Extract valid archive URLs using regex
    std::wregex urlRegex(L"\\b(\\d{8})/\\d{6}-\\d+-\\d+\\.mp3\\b");
    std::wsmatch match;
    std::set<std::wstring> uniqueDates;

    auto begin = std::wsregex_iterator(output.begin(), output.end(), urlRegex);
    auto end = std::wsregex_iterator();
    for (auto it = begin; it != end; ++it) {
        std::wstring fullMatch = it->str();
        std::size_t pos = fullMatch.find(L'/');
        if (pos != std::wstring::npos) {
            uniqueDates.insert(fullMatch.substr(0, pos)); // Extract the date part
        }
    }

    return std::vector<std::wstring>(uniqueDates.begin(), uniqueDates.end());
}

void presentDateMenu(const std::wstring& baseUrl) {
    std::vector<std::wstring> dates = crawlArchiveDates(baseUrl);

    if (dates.empty()) {
        std::wcerr << L"No valid dates found in the archive.\n";
        return;
    }

    std::wcout << L"Available dates:\n";
    for (size_t i = 0; i < dates.size(); ++i) {
        std::wcout << i + 1 << L". " << dates[i] << L"\n";
    }

    std::wcout << L"Enter the number(s) of the date(s) you want to download (comma-separated): ";
    std::wstring input;
    std::wcin.ignore(); // Clear input buffer
    std::getline(std::wcin, input);

    std::wstringstream ss(input);
    std::wstring token;
    std::vector<int> selectedIndices;

    while (std::getline(ss, token, L',')) {
        try {
            int index = std::stoi(token);
            if (index > 0 && index <= static_cast<int>(dates.size())) {
                selectedIndices.push_back(index - 1);
            }
        } catch (...) {
            std::wcerr << L"Invalid input: " << token << L"\n";
        }
    }

    for (int index : selectedIndices) {
        std::wstring date = dates[index];
        std::wstring archiveUrl = baseUrl + L"/" + date + L"/";
        std::wcout << L"Downloading archives for date: " << date << L"\n";
        downloadFeedArchives(NULL, NULL, archiveUrl);
    }
}
