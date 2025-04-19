#define UNICODE // Ensure UNICODE is defined
#include "app_logic.h"
#include <windows.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <CommCtrl.h>
#include <fstream>
#include <commdlg.h>
#include <shlobj.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// Helper function to convert std::wstring to std::string
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

void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& feedUrl) {
    std::wstring curlPath = L"bin\\curl.exe";
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

void listAvailableArchives(HWND hWnd, const std::wstring& feedUrl) {
    std::wstring curlPath = L"bin\\curl.exe";
    std::wstring tempFile = L"archives/temp_page.html";
    std::wstring command = curlPath + L" -o " + tempFile + L" " + feedUrl;

    if (_wsystem(command.c_str()) == 0) {
        std::wifstream inputFile(tempFile);
        if (!inputFile) {
            MessageBoxW(hWnd, L"Failed to open the downloaded page.", L"Error", MB_ICONERROR);
            return;
        }

        std::wstring line;
        std::wstring archivesList = L"Available Archives:\n";
        while (std::getline(inputFile, line)) {
            size_t pos = line.find(L".zip");
            if (pos != std::wstring::npos) {
                size_t start = line.rfind(L"\"", pos);
                size_t end = line.find(L"\"", pos);
                if (start != std::wstring::npos && end != std::wstring::npos) {
                    archivesList += line.substr(start + 1, end - start - 1) + L"\n";
                }
            }
        }
        inputFile.close();
        MessageBoxW(hWnd, archivesList.c_str(), L"Archives", MB_OK);
    } else {
        MessageBoxW(hWnd, L"Failed to fetch the webpage.", L"Error", MB_ICONERROR);
    }

    _wremove(tempFile.c_str()); // Clean up temporary file
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static Gdiplus::Image* backgroundImage = nullptr;

    switch (message) {
        case WM_CREATE: {
            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
            ULONG_PTR gdiplusToken;
            Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
            backgroundImage = Gdiplus::Image::FromFile(L"bin\\bkg-demo.jpg");
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            Gdiplus::Graphics graphics(hdc);
            if (backgroundImage) {
                graphics.DrawImage(backgroundImage, 0, 0, 1920, 1080);
            }

            Gdiplus::SolidBrush buttonBrush(Gdiplus::Color(255, 0, 122, 204));
            graphics.FillRectangle(&buttonBrush, 100, 200, 300, 100);
            graphics.FillRectangle(&buttonBrush, 100, 350, 300, 100);
            graphics.FillRectangle(&buttonBrush, 100, 500, 300, 100);

            Gdiplus::FontFamily fontFamily(L"Segoe UI");
            Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
            graphics.DrawString(L"List Archives", -1, &font, Gdiplus::PointF(120, 220), &textBrush);
            graphics.DrawString(L"Download Archives", -1, &font, Gdiplus::PointF(120, 370), &textBrush);
            graphics.DrawString(L"Transcribe Audio", -1, &font, Gdiplus::PointF(120, 520), &textBrush);

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_DESTROY: {
            delete backgroundImage;
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void presentOptions(HWND hWnd) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ModernUI";

    RegisterClass(&wc);

    HWND hMainWnd = CreateWindowEx(
        0, L"ModernUI", L"FeedFetcher", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(hMainWnd, SW_SHOWMAXIMIZED);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
