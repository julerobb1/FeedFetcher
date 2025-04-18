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
#include <shellapi.h>
#include <locale>
#include <codecvt>
#include <urlmon.h> // For URLDownloadToFile
#include <uxtheme.h> // For enabling visual styles
#include <dwmapi.h>  // For DwmExtendFrameIntoClientArea
#pragma comment(lib, "Dwmapi.lib") // Link with Dwmapi.lib
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef IDC_PROGRESS
#define IDC_PROGRESS 1002
#define BASE_URL L"https://archives.broadcastify.com/"
#endif

// Forward declarations
std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl, const std::wstring& feedId, const std::wstring& date);
void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile);
void presentDateMenu(const std::wstring& baseUrl);

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

    return true;
}

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

void OnTranscribeButtonClick(const std::wstring& inputFile, bool trimSilence) {
    MessageBox(NULL, 
        L"The transcription functionality requires Python and specific modules. If Python or the modules are not installed, the program will attempt to install them automatically.", 
        L"Transcription Disclaimer", 
        MB_ICONINFORMATION);

    if (!CheckAndInstallPythonModules()) {
        return;
    }

    std::wstring engine = L"pocketsphinx"; // Default to PocketSphinx
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    if (sysInfo.dwNumberOfProcessors >= 4) {
        engine = L"whisper"; // Use Whisper if the system is powerful
    }

    std::wstring command = L"python transcribe.py " + engine + L" " + inputFile;
    if (trimSilence) {
        command += L" --trim-silence";
    }

    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command).c_str(), NULL, SW_SHOWNORMAL);
}

void OnCombineButtonClick(const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile) {
    std::wstringstream command;
    command << L"ffmpeg -y -i \"concat:";
    for (size_t i = 0; i < inputFiles.size(); ++i) {
        command << inputFiles[i];
        if (i < inputFiles.size() - 1) {
            command << L"|";
        }
    }
    command << L"\" -c copy " << outputFile;

    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command.str()).c_str(), NULL, SW_SHOWNORMAL);
}

void LoadCustomBackground(HWND hwnd) {
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, L"background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBitmap) {
        HWND hStatic = CreateWindowW(
            L"STATIC", 
            NULL, 
            WS_VISIBLE | WS_CHILD | SS_BITMAP,
            0, 0, 0, 0,
            hwnd, NULL, 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
            NULL
        );
        SendMessage(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    } else {
        std::cerr << "Failed to load background image. Ensure 'background.bmp' exists and is valid." << std::endl;
    }
}

void UpdateProgressBar(HWND hProgressBar, int progress) {
    SendMessage(hProgressBar, PBM_SETPOS, progress, 0);
}

void HandleCombineFiles(HWND hWnd, const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile, bool trimSilence) {
    UpdateProgressBar(hProgressBar, 0);

    std::wstringstream command;
    command << L"ffmpeg -y -i \"concat:";
    for (size_t i = 0; i < inputFiles.size(); ++i) {
        command << inputFiles[i];
        if (i < inputFiles.size() - 1) {
            command << L"|";
        }
    }
    command << L"\" -c copy " << outputFile;

    int result = _wsystem(command.str().c_str());
    if (result != 0) {
        MessageBox(hWnd, L"Failed to combine files using FFmpeg.", L"Error", MB_ICONERROR);
        return;
    }

    if (trimSilence) {
        std::wstring pythonCommand = L"python python_scripts/transcribe.py --trim-silence \"" + outputFile + L"\"";
        result = _wsystem(pythonCommand.c_str());
        if (result != 0) {
            MessageBox(hWnd, L"Failed to trim silence using pydub.", L"Error", MB_ICONERROR);
            return;
        }
    }

    UpdateProgressBar(hProgressBar, 100);
    MessageBox(hWnd, L"Combine Files process completed successfully.", L"Success", MB_ICONINFORMATION);
}

// Function to check and install Python modules
bool CheckAndInstallPythonModules() {
    // Check if Python is installed
    if (_wsystem(L"python --version") != 0) {
        int response = MessageBox(NULL, 
            L"Python is not installed. The program will attempt to install Python and the required modules. Do you want to proceed?", 
            L"Python Installation Required", 
            MB_ICONWARNING | MB_YESNO);

        if (response == IDNO) {
            return false;
        }

        // Install Python (you can replace this with your actual installation logic)
        MessageBox(NULL, L"Python installation is not implemented in this example.", L"Error", MB_ICONERROR);
        return false;
    }

    // Install required Python modules
    int result = _wsystem(L"python -m pip install --upgrade pip && python -m pip install pydub pocketsphinx openai-whisper ffmpeg-python");
    if (result != 0) {
        MessageBox(NULL, L"Failed to install required Python modules. Please check your Python installation.", L"Error", MB_ICONERROR);
        return false;
    }

    MessageBox(NULL, L"Required Python modules have been successfully installed.", L"Success", MB_ICONINFORMATION);
    return true;
}

// Function to create subtle rounded corners
void SetSubtleRoundedCorners(HWND hWnd) {
    const int cornerRadius = 10; // Adjust this value for more or less rounding

    // Get the window dimensions
    RECT rect;
    GetWindowRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Create a rounded region
    HRGN hRgn = CreateRoundRectRgn(0, 0, width, height, cornerRadius, cornerRadius);

    // Apply the region to the window
    SetWindowRgn(hWnd, hRgn, TRUE);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hBaseUrlEdit, hDescriptionLabel, hDateList, hDownloadButton, hTranscribeButton, hCombineButton, hTrimSilenceCheckbox, hProgressBar, hBackground;

    switch (msg) {
        case WM_CREATE: {
            LoadCustomBackground(hWnd);
            SetSubtleRoundedCorners(hWnd);
            SetWindowTheme(hWnd, L"Explorer", NULL);

            hDescriptionLabel = CreateWindowW(
                L"STATIC", 
                L"Enter the numerical feed ID:",
                WS_VISIBLE | WS_CHILD,
                10, 10, 580, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hBaseUrlEdit = CreateWindowW(
                L"EDIT", 
                L"https://www.broadcastify.com/archives/feed/",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                10, 40, 580, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hDateList = CreateWindowW(
                L"LISTBOX", 
                NULL,
                WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                10, 70, 580, 150,
                hWnd, (HMENU)IDC_PROGRESS,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hDownloadButton = CreateWindowW(
                L"BUTTON", 
                L"Download",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                10, 230, 100, 30,
                hWnd, (HMENU)2,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hTranscribeButton = CreateWindowW(
                L"BUTTON", 
                L"Transcribe",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                120, 230, 100, 30,
                hWnd, (HMENU)3,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hCombineButton = CreateWindowW(
                L"BUTTON", 
                L"Combine Files",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                230, 230, 100, 30,
                hWnd, (HMENU)5,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hTrimSilenceCheckbox = CreateWindowW(
                L"BUTTON", 
                L"Trim Silence",
                WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                340, 230, 100, 30,
                hWnd, (HMENU)4,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            hProgressBar = CreateWindowW(
                PROGRESS_CLASS, 
                NULL,
                WS_VISIBLE | WS_CHILD,
                10, 270, 580, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            return 0;
        }
        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            MoveWindow(hBackground, 0, 0, width, height, TRUE);
            MoveWindow(hDescriptionLabel, 10, 10, width - 20, 20, TRUE);
            MoveWindow(hBaseUrlEdit, 10, 40, width - 20, 20, TRUE);
            MoveWindow(hDateList, 10, 70, width - 20, height - 180, TRUE);
            MoveWindow(hDownloadButton, 10, height - 100, 100, 30, TRUE);
            MoveWindow(hTranscribeButton, 120, height - 100, 100, 30, TRUE);
            MoveWindow(hCombineButton, 230, height - 100, 100, 30, TRUE);
            MoveWindow(hTrimSilenceCheckbox, 340, height - 100, 100, 30, TRUE);
            MoveWindow(hProgressBar, 10, height - 60, width - 20, 20, TRUE);

            SetSubtleRoundedCorners(hWnd);
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) { // Fetch Dates button clicked
                wchar_t feedId[512];
                GetWindowTextW(hBaseUrlEdit, feedId, 512);

                SYSTEMTIME st;
                GetLocalTime(&st);
                wchar_t date[16];
                swprintf(date, 16, L"%02d/%02d/%04d", st.wMonth, st.wDay, st.wYear);

                std::vector<std::wstring> dates = crawlArchiveDates(BASE_URL, feedId, date);
                SendMessageW(hDateList, LB_RESETCONTENT, 0, 0);

                if (dates.empty()) {
                    MessageBoxW(hWnd, L"No dates found. Please check the feed ID or your internet connection.", L"Error", MB_ICONERROR);
                } else {
                    for (const auto& date : dates) {
                        SendMessageW(hDateList, LB_ADDSTRING, 0, (LPARAM)date.c_str());
                    }
                }
            } else if (LOWORD(wParam) == 2) { // Download button clicked
                int count = SendMessageW(hDateList, LB_GETSELCOUNT, 0, 0);
                if (count > 0) {
                    std::vector<int> selectedIndices(count);
                    SendMessageW(hDateList, LB_GETSELITEMS, count, (LPARAM)selectedIndices.data());

                    wchar_t feedId[512];
                    GetWindowTextW(hBaseUrlEdit, feedId, 512);

                    std::wstring fullUrl = BASE_URL + std::wstring(feedId);

                    for (int index : selectedIndices) {
                        wchar_t date[16];
                        SendMessageW(hDateList, LB_GETTEXT, index, (LPARAM)date);

                        std::wstring archiveUrl = fullUrl + L"/" + date + L"/";
                        downloadFeedArchives(hWnd, hProgressBar, archiveUrl);
                    }
                }
            } else if (LOWORD(wParam) == 3) { // Transcribe button clicked
                wchar_t inputFile[512];
                GetWindowTextW(hBaseUrlEdit, inputFile, 512);

                bool trimSilence = SendMessageW(hTrimSilenceCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;

                OnTranscribeButtonClick(inputFile, trimSilence);
            } else if (LOWORD(wParam) == 5) { // Combine button clicked
                ShowCombineFilesWindow(hWnd);
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void ShowCombineFilesWindow(HWND parent) {
    HWND hCombineWnd = CreateWindowW(
        L"STATIC",
        L"Combine Files",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        parent, NULL,
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
        NULL
    );

    if (!hCombineWnd) {
        MessageBoxW(NULL, L"Failed to create Combine Files window.", L"Error", MB_ICONERROR);
        return;
    }

    ShowWindow(hCombineWnd, SW_SHOW);
    UpdateWindow(hCombineWnd);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

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
