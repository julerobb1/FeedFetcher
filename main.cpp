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
#include <cstdio> // For _pclose
#include "json.hpp" // Include nlohmann/json from vcpkg or system path
using json = nlohmann::json;.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "urlmon.lib"))
#pragma comment(lib, "Shlwapi.lib"))
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "UxTheme.lib")ependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#ifndef IDC_PROGRESS
#ifndef IDC_PROGRESS 1002
#define IDC_PROGRESS 1002//www.broadcastify.com/archives/feed/"
#define BASE_URL L"https://www.broadcastify.com/archives/feed/"
#endif
// Forward declarations
// Forward declarationsg> crawlArchiveDates(const std::wstring& baseUrl);
std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl);ile);
void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile);
bool CheckAndInstallPythonModules();td::wstring& inputFile, bool trimSilence);
void OnTranscribeButtonClick(const std::wstring& inputFile, bool trimSilence);td::wstring& outputFile);
void OnCombineButtonClick(const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile);
std::wstring fetchJsonData(const std::wstring& url);wstring& jsonData);
std::vector<std::wstring> parseJsonDates(const std::wstring& jsonData);
static HWND hProgressBar;
static HWND hProgressBar;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {nscribeButton, hCombineButton, hTrimSilenceCheckbox;
    static HWND hBaseUrlEdit, hDescriptionLabel, hDateList, hDownloadButton, hTranscribeButton, hCombineButton, hTrimSilenceCheckbox;
    switch (msg) {
    switch (msg) {EATE: {
        case WM_CREATE: {ual styles for all controls
            // Enable visual styles for all controls
            SetWindowTheme(hWnd, L"Explorer", NULL);
            // Create UI elements
            // Create UI elementsreateWindowW(L"STATIC", L"Enter the numerical feed ID:", WS_VISIBLE | WS_CHILD, 10, 10, 580, 20, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            hDescriptionLabel = CreateWindowW(L"STATIC", L"Enter the numerical feed ID:", WS_VISIBLE | WS_CHILD, 10, 10, 580, 20, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);LongPtr(hWnd, GWLP_HINSTANCE), NULL);
            hBaseUrlEdit = CreateWindowW(L"EDIT", L"https://www.broadcastify.com/archives/feed/", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 10, 40, 580, 20, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            hDateList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY, 10, 70, 580, 150, hWnd, (HMENU)IDC_PROGRESS, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            hDownloadButton = CreateWindowW(L"BUTTON", L"Download", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 230, 100, 30, hWnd, (HMENU)2, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);ULL);
            hTranscribeButton = CreateWindowW(L"BUTTON", L"Transcribe", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 230, 100, 30, hWnd, (HMENU)3, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            hCombineButton = CreateWindowW(L"BUTTON", L"Combine Files", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 230, 230, 100, 30, hWnd, (HMENU)5, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);LL);
            hTrimSilenceCheckbox = CreateWindowW(L"BUTTON", L"Trim Silence", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 340, 230, 100, 30, hWnd, (HMENU)4, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            hProgressBar = CreateWindowW(PROGRESS_CLASS, NULL, WS_VISIBLE | WS_CHILD, 10, 270, 580, 20, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
            return 0;
            return 0;
        }ase WM_COMMAND: {
        case WM_COMMAND: {ram) == 2) { // Download button clicked
            if (LOWORD(wParam) == 2) { // Download button clicked
                wchar_t feedId[512];UrlEdit, feedId, 512);
                GetWindowTextW(hBaseUrlEdit, feedId, 512);ring(feedId);
                std::wstring fullUrl = BASE_URL + std::wstring(feedId);lUrl);
                std::vector<std::wstring> dates = crawlArchiveDates(fullUrl);
                SendMessageW(hDateList, LB_RESETCONTENT, 0, 0);
                for (const auto& date : dates) {DDSTRING, 0, (LPARAM)date.c_str());
                    SendMessageW(hDateList, LB_ADDSTRING, 0, (LPARAM)date.c_str());
                }e if (LOWORD(wParam) == 3) { // Transcribe button clicked
            } else if (LOWORD(wParam) == 3) { // Transcribe button clicked
                wchar_t inputFile[512];Edit, inputFile, 512);
                GetWindowTextW(hBaseUrlEdit, inputFile, 512);heckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
                bool trimSilence = SendMessageW(hTrimSilenceCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
                OnTranscribeButtonClick(inputFile, trimSilence);clicked
            } else if (LOWORD(wParam) == 5) { // Combine button clicked"file2.mp3"};
                std::vector<std::wstring> inputFiles = {L"file1.mp3", L"file2.mp3"};
                std::wstring outputFile = L"combined.mp3";e);
                OnCombineButtonClick(inputFiles, outputFile);
            }eturn 0;
            return 0;
        }ase WM_DESTROY:
        case WM_DESTROY:age(0);
            PostQuitMessage(0);
            return 0;
    }eturn DefWindowProcW(hWnd, msg, wParam, lParam);
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;OMMONCONTROLSEX);
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    WNDCLASSW wc = {};
    WNDCLASSW wc = {};ainWndProc;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;DownloaderClass";
    wc.lpszClassName = L"FeedDownloaderClass";
    RegisterClassW(&wc);
    HWND hWnd = CreateWindowW(wc.lpszClassName, L"Feed Downloader", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    HWND hWnd = CreateWindowW(wc.lpszClassName, L"Feed Downloader", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    if (!hWnd) {oxW(NULL, L"Failed to create the main window.", L"Error", MB_ICONERROR);
        MessageBoxW(NULL, L"Failed to create the main window.", L"Error", MB_ICONERROR);
        return -1;
    }
    ShowWindow(hWnd, nCmdShow);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    MSG msg;
    MSG msg;etMessage(&msg, NULL, 0, 0)) {
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
    return 0;
}
bool InstallPython() {
bool InstallPython() {on installer
    // URL to the Python installerUrl = L"https://www.python.org/ftp/python/3.11.5/python-3.11.5-amd64.exe";
    const wchar_t* pythonInstallerUrl = L"https://www.python.org/ftp/python/3.11.5/python-3.11.5-amd64.exe";
    const wchar_t* installerPath = L"python-installer.exe";
    // Download the Python installer
    // Download the Python installerULL, pythonInstallerUrl, installerPath, 0, NULL);
    HRESULT hr = URLDownloadToFile(NULL, pythonInstallerUrl, installerPath, 0, NULL);
    if (FAILED(hr)) {LL, L"Failed to download Python installer. Please check your internet connection.", L"Error", MB_ICONERROR);
        MessageBox(NULL, L"Failed to download Python installer. Please check your internet connection.", L"Error", MB_ICONERROR);
        return false;
    }
    // Run the installer silently
    // Run the installer silentlyon-installer.exe /quiet InstallAllUsers=1 PrependPath=1";
    std::wstring command = L"python-installer.exe /quiet InstallAllUsers=1 PrependPath=1";
    int result = _wsystem(command.c_str());
    if (result != 0) {L, L"Failed to install Python. Please run the installer manually.", L"Error", MB_ICONERROR);
        MessageBox(NULL, L"Failed to install Python. Please run the installer manually.", L"Error", MB_ICONERROR);
        return false;
    }
    MessageBox(NULL, L"Python has been successfully installed.", L"Success", MB_ICONINFORMATION);
    MessageBox(NULL, L"Python has been successfully installed.", L"Success", MB_ICONINFORMATION);
    return true;
}
bool CheckAndInstallPythonModules() {
bool CheckAndInstallPythonModules() {
    // Check if Python is installed!= 0) {
    if (system("python --version") != 0) {
        int response = MessageBox(NULL, he program will attempt to install Python and the required modules in the background. Your user account MUST be an Administrator or have the ability to install programs on this machine. Do you want to proceed? No changes will be made to your device without your permission.", 
            L"Python is not installed. The program will attempt to install Python and the required modules in the background. Your user account MUST be an Administrator or have the ability to install programs on this machine. Do you want to proceed? No changes will be made to your device without your permission.", 
            L"Python Installation Required", 
            MB_ICONWARNING | MB_YESNO);
        if (response == IDNO) {
            return false;
        }f (!InstallPython()) {
        if (!InstallPython()) {
            return false;
        }
    }
    // Convert the command from std::wstring to std::string
    // Convert the command from std::wstring to std::stringde pip && python -m pip install pydub pocketsphinx openai-whisper ffmpeg-python";
    std::wstring commandW = L"python -m pip install --upgrade pip && python -m pip install pydub pocketsphinx openai-whisper ffmpeg-python";
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string command = converter.to_bytes(commandW);
    // Execute the command
    // Execute the commandmmand.c_str());
    int result = system(command.c_str());
    if (result != 0) {L, L"Failed to install required Python modules. Please check your Python installation and try again.", L"Error", MB_ICONERROR);
        MessageBox(NULL, L"Failed to install required Python modules. Please check your Python installation and try again.", L"Error", MB_ICONERROR);
        return false;
    }
    MessageBox(NULL, L"Required Python modules have been successfully installed.", L"Success", MB_ICONINFORMATION);
    MessageBox(NULL, L"Required Python modules have been successfully installed.", L"Success", MB_ICONINFORMATION);
    return true;
}
void OnTranscribeButtonClick(const std::wstring& inputFile, bool trimSilence) {
void OnTranscribeButtonClick(const std::wstring& inputFile, bool trimSilence) {
    // Show disclaimer
    MessageBox(NULL, iption functionality requires Python and specific modules. If Python or the modules are not installed, the program will attempt to install them automatically.", 
        L"The transcription functionality requires Python and specific modules. If Python or the modules are not installed, the program will attempt to install them automatically.", 
        L"Transcription Disclaimer", 
        MB_ICONINFORMATION);
    // Ensure Python modules are installed
    // Ensure Python modules are installed
    if (!CheckAndInstallPythonModules()) {
        return;
    }
    // Decide engine based on system specifications
    // Decide engine based on system specificationslt to PocketSphinx
    std::wstring engine = L"pocketsphinx"; // Default to PocketSphinx
    SYSTEM_INFO sysInfo;fo);
    GetSystemInfo(&sysInfo);
    // Check for GPU (basic heuristic)
    // Check for GPU (basic heuristic)= 4) {
    if (sysInfo.dwNumberOfProcessors >= 4) {if the system is powerful
        engine = L"whisper"; // Use Whisper if the system is powerful
    }
    // Add silence trimming flag if selected
    // Add silence trimming flag if selectedbe.py " + engine + L" " + inputFile;
    std::wstring command = L"python transcribe.py " + engine + L" " + inputFile;
    if (trimSilence) {--trim-silence";
        command += L" --trim-silence";
    }
    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command).c_str(), NULL, SW_SHOWNORMAL);
    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command).c_str(), NULL, SW_SHOWNORMAL);
}
void OnCombineButtonClick(const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile) {
void OnCombineButtonClick(const std::vector<std::wstring>& inputFiles, const std::wstring& outputFile) {
    // Combine files using FFmpeg
    std::wstringstream command;"concat:";
    command << L"ffmpeg -y -i \"concat:";e(); ++i) {
    for (size_t i = 0; i < inputFiles.size(); ++i) {
        command << inputFiles[i]; - 1) {
        if (i < inputFiles.size() - 1) {
            command << L"|";
        }
    }ommand << L"\" -c copy " << outputFile;
    command << L"\" -c copy " << outputFile;
    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command.str()).c_str(), NULL, SW_SHOWNORMAL);
    ShellExecute(NULL, L"open", L"cmd.exe", (L"/C " + command.str()).c_str(), NULL, SW_SHOWNORMAL);
}
void LoadCustomBackground(HWND hwnd) {
void LoadCustomBackground(HWND hwnd) {
    // Load the custom backgroundadImage(NULL, L"background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, L"background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBitmap) { background
        // Set the backgroundWindowW(
        HWND hStatic = CreateWindowW(
            L"STATIC", 
            NULL, IBLE | WS_CHILD | SS_BITMAP,
            WS_VISIBLE | WS_CHILD | SS_BITMAP,
            0, 0, 0, 0, 
            hwnd, NULL, etWindowLongPtr(hwnd, GWLP_HINSTANCE), 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
            NULL
        );ndMessage(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
        SendMessage(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    } else {:cerr << "Failed to load background image. Ensure 'background.bmp' exists and is valid." << std::endl;
        std::cerr << "Failed to load background image. Ensure 'background.bmp' exists and is valid." << std::endl;
    }
}
void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile) {
void runFFmpeg(const std::wstring& inputFile, const std::wstring& outputFile) {
    std::wstring ffmpegPath = L"bin\\ffmpeg.exe"; // Updated pathutFile + L"\" \"" + outputFile + L"\"";
    std::wstring command = L"\"" + ffmpegPath + L"\" -i \"" + inputFile + L"\" \"" + outputFile + L"\"";
    int result = _wsystem(command.c_str());
    if (result != 0) {L"FFmpeg command failed with error code: " << result << std::endl;
        std::wcerr << L"FFmpeg command failed with error code: " << result << std::endl;
    }
}
std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl) {
std::vector<std::wstring> crawlArchiveDates(const std::wstring& baseUrl) {
    // Fetch JSON data from the serverata(baseUrl);
    std::wstring jsonData = fetchJsonData(baseUrl);
    // Parse the JSON data to extract dates
    // Parse the JSON data to extract dates
    return parseJsonDates(jsonData);
}
void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& archiveUrl) {
void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& archiveUrl) {
    std::wstring curlPath = L"bin\\curl.exe"; // Path to curl executable L"\"";
    std::wstring command = L"\"" + curlPath + L"\" -O \"" + archiveUrl + L"\"";
    // Run curl to download the archive
    // Run curl to download the archive());
    int result = _wsystem(command.c_str());
    if (result != 0) {d, L"Failed to download archive. Please check your internet connection.", L"Error", MB_ICONERROR);
        MessageBox(hWnd, L"Failed to download archive. Please check your internet connection.", L"Error", MB_ICONERROR);
    } else {ageBox(hWnd, L"Archive downloaded successfully.", L"Success", MB_ICONINFORMATION);
        MessageBox(hWnd, L"Archive downloaded successfully.", L"Success", MB_ICONINFORMATION);
    }
    // Update progress bar (if applicable)
    // Update progress bar (if applicable)
    if (hProgressBar) {ogressBar, PBM_SETPOS, 100, 0);
        SendMessage(hProgressBar, PBM_SETPOS, 100, 0);
    }
}
void presentDateMenu(const std::wstring& baseUrl) {
void presentDateMenu(const std::wstring& baseUrl) {ates(baseUrl);
    std::vector<std::wstring> dates = crawlArchiveDates(baseUrl);
    if (dates.empty()) {
    if (dates.empty()) {No valid dates found in the archive.\n";
        std::wcerr << L"No valid dates found in the archive.\n";
        return;
    }
    std::wcout << L"Available dates:\n";
    std::wcout << L"Available dates:\n"; ++i) {
    for (size_t i = 0; i < dates.size(); ++i) {] << L"\n";
        std::wcout << i + 1 << L". " << dates[i] << L"\n";
    }
    std::wcout << L"Enter the number(s) of the date(s) you want to download (comma-separated): ";
    std::wcout << L"Enter the number(s) of the date(s) you want to download (comma-separated): ";
    std::wstring input; // Clear input buffer
    std::wcin.ignore(); // Clear input buffer
    std::getline(std::wcin, input);
    std::wstringstream ss(input);
    std::wstringstream ss(input);
    std::wstring token;lectedIndices;
    std::vector<int> selectedIndices;
    while (std::getline(ss, token, L',')) {
    while (std::getline(ss, token, L',')) {
        try {nt index = std::stoi(token);
            int index = std::stoi(token);tic_cast<int>(dates.size())) {
            if (index > 0 && index <= static_cast<int>(dates.size())) {
                selectedIndices.push_back(index - 1);
            }ch (...) {
        } catch (...) {<< L"Invalid input: " << token << L"\n";
            std::wcerr << L"Invalid input: " << token << L"\n";
        }
    }
    for (int index : selectedIndices) {
    for (int index : selectedIndices) {];
        std::wstring date = dates[index]; + L"/" + date + L"/";
        std::wstring archiveUrl = baseUrl + L"/" + date + L"/";e << L"\n";
        std::wcout << L"Downloading archives for date: " << date << L"\n";
        downloadFeedArchives(NULL, NULL, archiveUrl);
    }
}
std::wstring fetchJsonData(const std::wstring& url) {
std::wstring fetchJsonData(const std::wstring& url) { to curl executable
    std::wstring curlPath = L"bin\\curl.exe"; // Path to curl executable
    std::wstring command = L"\"" + curlPath + L"\" -s \"" + url + L"\"";
    std::wstring output;
    FILE* pipe = _wpopen(command.c_str(), L"r");
    FILE* pipe = _wpopen(command.c_str(), L"r");
    if (!pipe) {rr << L"Failed to run curl command.\n";
        std::wcerr << L"Failed to run curl command.\n";
        return L"";
    }
    wchar_t buffer[128];
    wchar_t buffer[128];, 128, pipe)) {
    while (fgetws(buffer, 128, pipe)) {
        output += buffer;
    }
    _pclose(pipe); // Ensure _pclose is defined by including <cstdio>
    _pclose(pipe); // Ensure _pclose is defined by including <cstdio>
    return output;
}
// Function to parse JSON data
// Function to parse JSON dataeJsonDates(const std::wstring& jsonData) {
std::vector<std::wstring> parseJsonDates(const std::wstring& jsonData) {
    std::vector<std::wstring> dates;
    try {/ Convert std::wstring to std::string
        // Convert std::wstring to std::string<wchar_t>> converter;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string jsonString = converter.to_bytes(jsonData);
        // Parse the JSON string using nlohmann/json
        // Parse the JSON string using nlohmann/json/ Use 'json', not 'nlohmann::json'
        json parsedJson = json::parse(jsonString); // Use 'json', not 'nlohmann::json'
        // Extract dates (assuming the JSON structure contains a "dates" array)
        // Extract dates (assuming the JSON structure contains a "dates" array)
        for (const auto& date : parsedJson["dates"]) {get<std::string>()));
            dates.push_back(converter.from_bytes(date.get<std::string>()));
        }ch (const std::exception& e) {
    } catch (const std::exception& e) { JSON: " << e.what() << std::endl;
        std::wcerr << L"Failed to parse JSON: " << e.what() << std::endl;
    }eturn dates;
    return dates;
}