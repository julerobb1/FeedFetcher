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
std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl);
void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile);
void presentDateMenu(const std::wstring& baseUrl); // Added forward declaration

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

bool InstallPython() {
    // URL to the Python installer
    const wchar_t* pythonInstallerUrl = L"https://www.python.org/ftp/python/3.11.5/python-3.11.5-amd64.exe";
    const wchar_t* installerPath = L"python-installer.exe";

    // Download the Python installer
    HRESULT hr = URLDownloadToFile(NULL, pythonInstallerUrl, installerPath, 0, NULL);
    if (FAILED(hr)) {
        MessageBox(NULL, L"Failed to download Python installer. Please check your internet connection.", L"Error", MB_ICONERROR);
        return false;
    }

    // Run the installer silently
    std::wstring command = L"python-installer.exe /quiet InstallAllUsers=1 PrependPath=1";
    int result = _wsystem(command.c_str());
    if (result != 0) {
        MessageBox(NULL, L"Failed to install Python. Please run the installer manually.", L"Error", MB_ICONERROR);
        return false;
    }

    MessageBox(NULL, L"Python has been successfully installed.", L"Success", MB_ICONINFORMATION);
    return true;
}

bool CheckAndInstallPythonModules() {
    // Check if Python is installed
    if (system("python --version") != 0) {
        int response = MessageBox(NULL, 
            L"Python is not installed. The program will attempt to install Python and the required modules in the background. Your user account MUST be an Administrator or have the ability to install programs on this machine. Do you want to proceed? No changes will be made to your device without your permission.", 
            L"Python Installation Required", 
            MB_ICONWARNING | MB_YESNO);

        if (response == IDNO) {
            return false;
        }

        if (!InstallPython()) {
            return false;
        }
    }

    // Convert the command from std::wstring to std::string
    std::wstring commandW = L"python -m pip install --upgrade pip && python -m pip install pydub pocketsphinx openai-whisper ffmpeg-python";
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string command = converter.to_bytes(commandW);

    // Execute the command
    int result = system(command.c_str());

    if (result != 0) {
        MessageBox(NULL, L"Failed to install required Python modules. Please check your Python installation and try again.", L"Error", MB_ICONERROR);
        return false;
    }

    MessageBox(NULL, L"Required Python modules have been successfully installed.", L"Success", MB_ICONINFORMATION);
    return true;
}

void OnTranscribeButtonClick(const std::wstring& inputFile, bool trimSilence) {
    // Show disclaimer
    MessageBox(NULL, 
        L"The transcription functionality requires Python and specific modules. If Python or the modules are not installed, the program will attempt to install them automatically.", 
        L"Transcription Disclaimer", 
        MB_ICONINFORMATION);

    // Ensure Python modules are installed
    if (!CheckAndInstallPythonModules()) {
        return;
    }

    // Decide engine based on system specifications
    std::wstring engine = L"pocketsphinx"; // Default to PocketSphinx
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    // Check for GPU (basic heuristic)
    if (sysInfo.dwNumberOfProcessors >= 4) {
        engine = L"whisper"; // Use Whisper if the system is powerful
    }

    // Add silence trimming flag if selected
    std::wstring command = L"python transcribe.py " + engine + L" " + inputFile;
    if (trimSilence) {
        command += L" --trim-silence";
    }

    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command).c_str(), NULL, SW_SHOWNORMAL);
}

void OnCombineButtonClick(const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile) {
    // Combine files using FFmpeg
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
    // Load the custom background
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, L"background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBitmap) {
        // Set the background
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

// Forward declaration for the Combine Files window
void ShowCombineFilesWindow(HWND parent);

// Function to update the progress bar
void UpdateProgressBar(HWND hProgressBar, int progress) {
    SendMessage(hProgressBar, PBM_SETPOS, progress, 0);
}

// Function to handle the Combine Files process
void HandleCombineFiles(HWND hWnd, const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile, bool trimSilence) {
    // Show progress bar
    UpdateProgressBar(hProgressBar, 0);

    // Combine files using FFmpeg
    std::wstringstream command;
    command << L"ffmpeg -y -i \"concat:";
    for (size_t i = 0; i < inputFiles.size(); ++i) {
        command << inputFiles[i];
        if (i < inputFiles.size() - 1) {
            command << L"|";
        }
    }
    command << L"\" -c copy " << outputFile;

    // Execute FFmpeg command
    int result = _wsystem(command.str().c_str());
    if (result != 0) {
        MessageBox(hWnd, L"Failed to combine files using FFmpeg.", L"Error", MB_ICONERROR);
        return;
    }

    // If Trim Silence is enabled, process the output file with pydub
    if (trimSilence) {
        std::wstring pythonCommand = L"python python_scripts/transcribe.py --trim-silence \"" + outputFile + L"\"";
        result = _wsystem(pythonCommand.c_str());
        if (result != 0) {
            MessageBox(hWnd, L"Failed to trim silence using pydub.", L"Error", MB_ICONERROR);
            return;
        }
    }

    // Update progress bar to 100%
    UpdateProgressBar(hProgressBar, 100);
    MessageBox(hWnd, L"Combine Files process completed successfully.", L"Success", MB_ICONINFORMATION);
}

// Function to reduce rounded edges on Windows 11
// void ReduceRoundedEdges(HWND hWnd) {
//     const int DWMWA_WINDOW_CORNER_PREFERENCE = 33;
//     const int DWM_WINDOW_CORNER_PREFERENCE_NONE = 1; // No rounded corners
//     DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &DWM_WINDOW_CORNER_PREFERENCE_NONE, sizeof(DWM_WINDOW_CORNER_PREFERENCE_NONE));
// }

// Function to create subtle rounded corners
void SetSubtleRoundedCorners(HWND hWnd) {
    // Define the corner radius (smaller for subtle rounding)
    const int cornerRadius = 5; // Adjust this value for more or less rounding

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
    static HWND hBaseUrlEdit, hDescriptionLabel, hDateList, hDownloadButton, hTranscribeButton, hCombineButton, hTrimSilenceCheckbox, hProgressBar;

    switch (msg) {
        case WM_CREATE: {
            // Apply subtle rounded corners
            SetSubtleRoundedCorners(hWnd);

            // Enable visual styles for all controls
            SetWindowTheme(hWnd, L"Explorer", NULL);

            // Adjust window size to fit all controls
            SetWindowPos(hWnd, NULL, 0, 0, 600, 400, SWP_NOMOVE | SWP_NOZORDER);

            // Create a label for the feed ID description
            hDescriptionLabel = CreateWindowW(
                L"STATIC", 
                L"Enter the numerical feed ID:",
                WS_VISIBLE | WS_CHILD,
                10, 10, 580, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create an edit box for the feed ID with prefilled URL
            hBaseUrlEdit = CreateWindowW(
                L"EDIT", 
                L"https://www.broadcastify.com/archives/feed/",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                10, 40, 580, 20,
                hWnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create a list box for available dates
            hDateList = CreateWindowW(
                L"LISTBOX", 
                NULL,
                WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                10, 70, 580, 150,
                hWnd, (HMENU)IDC_PROGRESS,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Create buttons
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

            // Create a progress bar
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
        case WM_PAINT: {
            // Force redraw to prevent black boxes
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_SIZE: {
            // Reapply subtle rounded corners on resize
            SetSubtleRoundedCorners(hWnd);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) { // Fetch Dates button clicked
                wchar_t feedId[512];
                GetWindowTextW(hBaseUrlEdit, feedId, 512);

                // Construct the full URL
                std::wstring fullUrl = BASE_URL + std::wstring(feedId);

                // Fetch available dates
                std::vector<std::wstring> dates = crawlArchiveDates(fullUrl);
                SendMessageW(hDateList, LB_RESETCONTENT, 0, 0); // Clear the list box

                for (const auto& date : dates) {
                    SendMessageW(hDateList, LB_ADDSTRING, 0, (LPARAM)date.c_str());
                }
            } else if (LOWORD(wParam) == 2) { // Download button clicked
                int count = SendMessageW(hDateList, LB_GETSELCOUNT, 0, 0);
                if (count > 0) {
                    std::vector<int> selectedIndices(count);
                    SendMessageW(hDateList, LB_GETSELITEMS, count, (LPARAM)selectedIndices.data());

                    wchar_t feedId[512];
                    GetWindowTextW(hBaseUrlEdit, feedId, 512);

                    // Construct the full URL
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

                // Check if silence trimming is selected
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

// Function to show the Combine Files window
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

    // Add controls for file selection, progress bar, and combine action
    // ... Add controls and logic here ...
}

// Enable visual styles for the application
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES; // Enable common controls
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

    std::wstring baseUrl = L"https://www.broadcastify.com/archives/feed/";
    presentDateMenu(baseUrl);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile) {
    std::wstring ffmpegPath = L"bin\\ffmpeg.exe"; // Updated path
    std::wstring command = L"\"" + ffmpegPath + L"\" -i \"" + inputFile + L"\" \"" + outputFile + L"\"";
    int result = _wsystem(command.c_str());
    if (result != 0) {
        std::wcerr << L"FFmpeg command failed with error code: " << result << std::endl;
    }
}

std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl) {
    std::wstring curlPath = L"bin\\curl.exe"; // Updated path
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
