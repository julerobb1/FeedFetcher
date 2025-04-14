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
#pragma comment(lib, "gdiplus.lib")
// Helper function to convert std::wstring to LPCSTR
std::string WStringToString(const std::wstring& wstr) {
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(sizeNeeded, 0);td::wstring& wstr) {
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], sizeNeeded, NULL, NULL);ULL);
    return str; str(sizeNeeded, 0);
}   WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], sizeNeeded, NULL, NULL);
    return str;
// Helper function to execute a system command
bool executeCommand(const std::wstring& command) {
    int result = _wsystem(command.c_str());and
    return result == 0;st std::wstring& command) {
}   int result = _wsystem(command.c_str());
    return result == 0;
void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& feedUrl) {
    std::wstring curlPath = L"c:/Users/juler/Downloads/curl-8.12.1_4-win64-mingw/curl-8.12.1_4-win64-mingw/bin/curl.exe";
void downloadFeedArchives(HWND hWnd, HWND hProgressBar, const std::wstring& feedUrl) {
    std::wstring outputFile = L"archives/feed_archive.zip";-8.12.1_4-win64-mingw/curl-8.12.1_4-win64-mingw/bin/curl.exe";
    std::wstring command = curlPath + L" -o " + outputFile + L" " + feedUrl;
    std::wstring outputFile = L"archives/feed_archive.zip";
    SendMessage(hProgressBar, PBM_SETPOS, 50, 0); // Update progress bar to 50%

    int result = _wsystem(command.c_str());0, 0); // Update progress bar to 50%
    if (result == 0) {
        SendMessage(hProgressBar, PBM_SETPOS, 100, 0); // Update progress bar to 100%
        MessageBoxW(hWnd, L"Feed archives downloaded successfully.", L"Success", MB_OK);
    } else {Message(hProgressBar, PBM_SETPOS, 100, 0); // Update progress bar to 100%
        MessageBoxW(hWnd, L"Failed to download feed archives.", L"Error", MB_ICONERROR);
    } else {
}       MessageBoxW(hWnd, L"Failed to download feed archives.", L"Error", MB_ICONERROR);
    }
void runTranscriber(const std::wstring& audioFilePath) {
    std::wstring command = L"transcriber_tool.exe " + audioFilePath + L" > transcription.txt";
    if (executeCommand(command)) {ring& audioFilePath) {
        MessageBoxW(NULL, L"Transcription completed successfully. Output saved to transcription.txt.", L"Info", MB_OK);
    } else {uteCommand(command)) {
        MessageBoxW(NULL, L"Failed to transcribe the audio file.", L"Error", MB_ICONERROR);tion.txt.", L"Info", MB_OK);
    } else {
}       MessageBoxW(NULL, L"Failed to transcribe the audio file.", L"Error", MB_ICONERROR);
    }
void trimSilence(const std::wstring& inputFile, const std::wstring& outputFile) {
    std::wstring command = L"ffmpeg -i " + inputFile + L" -af silenceremove=1:0:-50dB " + outputFile;
    if (executeCommand(command)) {g& inputFile, const std::wstring& outputFile) {
        MessageBoxW(NULL, L"Silence trimmed successfully.", L"Info", MB_OK);1:0:-50dB " + outputFile;
    } else {uteCommand(command)) {
        MessageBoxW(NULL, L"Failed to trim silence from the audio file.", L"Error", MB_ICONERROR);
    } else {
}       MessageBoxW(NULL, L"Failed to trim silence from the audio file.", L"Error", MB_ICONERROR);
    }
void analyzeAudio(const std::wstring& inputFile) {
    // Placeholder for future implementation
    MessageBoxW(NULL, L"Audio analysis is not yet implemented.", L"Info", MB_OK);
}   // Placeholder for future implementation
    MessageBoxW(NULL, L"Audio analysis is not yet implemented.", L"Info", MB_OK);
void combineFiles(const std::wstring& outputFile, const std::vector<std::wstring>& inputFiles) {
    std::wstring tempFileList = L"file_list.txt";
    FILE* fileList = _wfopen(tempFileList.c_str(), L"w");td::vector<std::wstring>& inputFiles) {
    if (!fileList) {pFileList = L"file_list.txt";
        MessageBoxW(NULL, L"Failed to create temporary file list.", L"Error", MB_ICONERROR);
        return;st) {
    }   MessageBoxW(NULL, L"Failed to create temporary file list.", L"Error", MB_ICONERROR);
        return;
    for (const auto& inputFile : inputFiles) {
        fwprintf(fileList, L"file '%s'\n", inputFile.c_str());
    }or (const auto& inputFile : inputFiles) {
    fclose(fileList);List, L"file '%s'\n", inputFile.c_str());
    }
    std::wstring command = L"ffmpeg -f concat -safe 0 -i " + tempFileList + L" -c copy " + outputFile;
    if (executeCommand(command)) {
        MessageBoxW(NULL, L"Files combined successfully.", L"Info", MB_OK); L" -c copy " + outputFile;
    } else {uteCommand(command)) {
        MessageBoxW(NULL, L"Failed to combine files.", L"Error", MB_ICONERROR);
    } else {
        MessageBoxW(NULL, L"Failed to combine files.", L"Error", MB_ICONERROR);
    _wremove(tempFileList.c_str()); // Clean up temporary file
}
    _wremove(tempFileList.c_str()); // Clean up temporary file
void listAvailableArchives(HWND hWnd, const std::wstring& feedUrl) {
    std::wstring curlPath = L"c:/Users/juler/Downloads/curl-8.12.1_4-win64-mingw/curl-8.12.1_4-win64-mingw/bin/curl.exe";
    std::wstring tempFile = L"archives/temp_page.html";g& feedUrl) {
    std::wstring command = curlPath + L" -o " + tempFile + L" " + feedUrl;-mingw/curl-8.12.1_4-win64-mingw/bin/curl.exe";
    std::wstring tempFile = L"archives/temp_page.html";
    if (_wsystem(command.c_str()) == 0) {-o " + tempFile + L" " + feedUrl;
        std::wifstream inputFile(tempFile);
        if (!inputFile) {c_str()) == 0) {
            MessageBoxW(hWnd, L"Failed to open the downloaded page.", L"Error", MB_ICONERROR);
            return;ile) {
        }   MessageBoxW(hWnd, L"Failed to open the downloaded page.", L"Error", MB_ICONERROR);
            return;
        std::wstring line;
        std::wstring archivesList = L"Available Archives:\n";
        while (std::getline(inputFile, line)) {
            size_t pos = line.find(L".zip");ble Archives:\n";
            if (pos != std::wstring::npos) {) {
                size_t start = line.rfind(L"\"", pos);
                size_t end = line.find(L"\"", pos);
                if (start != std::wstring::npos && end != std::wstring::npos) {
                    archivesList += line.substr(start + 1, end - start - 1) + L"\n";
                }f (start != std::wstring::npos && end != std::wstring::npos) {
            }       archivesList += line.substr(start + 1, end - start - 1) + L"\n";
        }       }
        inputFile.close();
        MessageBoxW(hWnd, archivesList.c_str(), L"Archives", MB_OK);
    } else {tFile.close();
        MessageBoxW(hWnd, L"Failed to fetch the webpage.", L"Error", MB_ICONERROR);
    } else {
        MessageBoxW(hWnd, L"Failed to fetch the webpage.", L"Error", MB_ICONERROR);
    _wremove(tempFile.c_str()); // Clean up temporary file
}
    _wremove(tempFile.c_str()); // Clean up temporary file
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static Gdiplus::Image* backgroundImage = nullptr;
void EnableAeroGlass(HWND hWnd) {
    switch (message) {{ -1 }; // Extend the glass effect to the entire window
        case WM_CREATE: {entArea(hWnd, &margins);
            // Initialize GDI+ and load the background image
            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
            ULONG_PTR gdiplusToken;nd, UINT message, WPARAM wParam, LPARAM lParam) {
            Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
            backgroundImage = Gdiplus::Image::FromFile(L"bin\\bkg-demo.jpg"); // Updated path
            break;FadeInEffect(hWnd); // Add fade-in effecte) {
        }   break;ase WM_CREATE: {
        case WM_PAINT: { the background image
            PAINTSTRUCT ps;artupInput gdiplusStartupInput;
            HDC hdc = BeginPaint(hWnd, &ps);
            HDC hdc = BeginPaint(hWnd, &ps);            Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
            // Use GDI+ to draw the background image
            Gdiplus::Graphics graphics(hdc);nd image Aero Glass effect
            if (backgroundImage) {hics(hdc);
                graphics.DrawImage(backgroundImage, 0, 0, 1920, 1080); // Adjust dimensions as needed
            }   graphics.DrawImage(backgroundImage, 0, 0, 1920, 1080); // Adjust dimensions as neededWM_PAINT: {
            }            PAINTSTRUCT ps;
            // Draw custom buttons
            Gdiplus::SolidBrush buttonBrush(Gdiplus::Color(255, 0, 122, 204)); // Blue color
            graphics.FillRectangle(&buttonBrush, 100, 200, 300, 100); // Button 1 Blue color;
            graphics.FillRectangle(&buttonBrush, 100, 350, 300, 100); // Button 2
            graphics.FillRectangle(&buttonBrush, 100, 500, 300, 100); // Button 3
            graphics.FillRectangle(&buttonBrush, 100, 500, 300, 100); // Button 3            EndPaint(hWnd, &ps);                graphics.DrawImage(backgroundImage, 0, 0, 1920, 1080); // Adjust dimensions as needed
            // Add button text
            Gdiplus::FontFamily fontFamily(L"Segoe UI");
            Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255)); // White color);
            graphics.DrawString(L"List Archives", -1, &font, Gdiplus::PointF(120, 220), &textBrush);
            graphics.DrawString(L"Download Archives", -1, &font, Gdiplus::PointF(120, 370), &textBrush);
            graphics.DrawString(L"Transcribe Audio", -1, &font, Gdiplus::PointF(120, 520), &textBrush);;
            graphics.DrawString(L"Transcribe Audio", -1, &font, Gdiplus::PointF(120, 520), &textBrush);            if (x >= 100 && x <= 400 && y >= 200 && y <= 300) {
            EndPaint(hWnd, &ps);fo", MB_OK);
            break;nt(hWnd, &ps); if (x >= 100 && x <= 400 && y >= 350 && y <= 450) {s::FontFamily fontFamily(L"Arial");
        }   break;       MessageBox(hWnd, L"Download Archives clicked!", L"Info", MB_OK);   Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        case WM_LBUTTONDOWN: { 500 && y <= 600) {h textBrush(Gdiplus::Color(255, 255, 255, 255)); // White color
            // Handle button clickscribe Audio clicked!", L"Info", MB_OK);ist Archives", -1, &font, Gdiplus::PointF(120, 220), &textBrush);
            int x = LOWORD(lParam); &font, Gdiplus::PointF(120, 370), &textBrush);
            int y = HIWORD(lParam); -1, &font, Gdiplus::PointF(120, 520), &textBrush);
            int y = HIWORD(lParam);        }
            if (x >= 100 && x <= 400 && y >= 200 && y <= 300) {
                MessageBox(hWnd, L"List Archives clicked!", L"Info", MB_OK);
            } else if (x >= 100 && x <= 400 && y >= 350 && y <= 450) {B_OK);
                MessageBox(hWnd, L"Download Archives clicked!", L"Info", MB_OK);
            } else if (x >= 100 && x <= 400 && y >= 500 && y <= 600) {", MB_OK);
                MessageBox(hWnd, L"Transcribe Audio clicked!", L"Info", MB_OK);
            }   MessageBox(hWnd, L"Transcribe Audio clicked!", L"Info", MB_OK);eturn DefWindowProc(hWnd, message, wParam, lParam);nt y = HIWORD(lParam);
            break;
        }   break;n 0;   if (x >= 100 && x <= 400 && y >= 200 && y <= 300) {
        case WM_DESTROY: { MB_OK);
            delete backgroundImage;
            PostQuitMessage(0);age;{, L"Download Archives clicked!", L"Info", MB_OK);
            break;itMessage(0);d create a full-screen window if (x >= 100 && x <= 400 && y >= 500 && y <= 600) {
        }   break;ASS wc = { 0 };       MessageBox(hWnd, L"Transcribe Audio clicked!", L"Info", MB_OK);
        default:ndowProc;
            return DefWindowProc(hWnd, message, wParam, lParam);
    }       return DefWindowProc(hWnd, message, wParam, lParam);c.lpszClassName = L"ModernUI"; // Use wide-character string   }
    return 0;
}   return 0;   RegisterClass(&wc);           delete backgroundImage;
}            PostQuitMessage(0);
void presentOptions(HWND hWnd) {
    // Register and create a full-screen windowW, // Use wide-character strings
    WNDCLASS wc = { 0 }; sizeof(ANIMATIONINFO), 1 }; // Enable animationsW_USEDEFAULT, 1920, 1080,
    wc.lpfnWndProc = WindowProc;ANIMATION, sizeof(ANIMATIONINFO), &ai, 0);dle(NULL), NULL);(hWnd, message, wParam, lParam);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ModernUI";TYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);}
    RegisterClass(&wc);
    for (BYTE alpha = 0; alpha <= 255; alpha += 5) {void presentOptions(HWND hWnd) {
    HWND hMainWnd = CreateWindowEx(hWnd, 0, alpha, LWA_ALPHA);ndow
        0, L"ModernUI", L"FeedFetcher", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
        NULL, NULL, GetModuleHandle(NULL), NULL);
        DispatchMessage(&msg);    wc.lpszClassName = L"ModernUI"; // Use wide-character string
    EnableAeroGlass(hMainWnd); // Enable Aero Glass
    EnableFadeInEffect(hMainWnd); // Add fade-in effect    // Register and create a full-screen window}    RegisterClass(&wc);

    ShowWindow(hMainWnd, SW_SHOWMAXIMIZED); WindowProc;on(Gdiplus::Graphics& graphics, int x, int y, int width, int height, const wchar_t* text) {CreateWindowEx(
INDOW, // Use wide-character strings
    // Run the message loopUI"; // Use wide-character string80, 90); // Top-left cornerFAULT, 1920, 1080,
    MSG msg = { 0 };right cornerandle(NULL), NULL);
    while (GetMessage(&msg, NULL, 0, 0)) {egisterClass(&wc);ath.AddArc(x + width - 20, y + height - 20, 20, 20, 0, 90); // Bottom-right corner
        TranslateMessage(&msg);  path.AddArc(x, y + height - 20, 20, 20, 90, 90); // Bottom-left corner   ShowWindow(hMainWnd, SW_SHOWMAXIMIZED);
        DispatchMessage(&msg);    HWND hMainWnd = CreateWindowEx(    path.CloseFigure();



}    }













}    }        DispatchMessage(&msg);        TranslateMessage(&msg);    while (GetMessage(&msg, NULL, 0, 0)) {    MSG msg = { 0 };    // Run the message loop    ShowWindow(hMainWnd, SW_SHOWMAXIMIZED);        NULL, NULL, GetModuleHandle(NULL), NULL);        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,        0, L"ModernUI", L"FeedFetcher", WS_OVERLAPPEDWINDOW, // Use wide-character strings














}    graphics.DrawString(text, -1, &font, rect, &format, &textBrush);    Gdiplus::RectF rect(x, y, width, height);    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);    format.SetAlignment(Gdiplus::StringAlignmentCenter);    Gdiplus::StringFormat format;    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255)); // White text    Gdiplus::Font font(&fontFamily, 16, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);    Gdiplus::FontFamily fontFamily(L"Segoe UI");    graphics.FillPath(&brush, &path);    Gdiplus::SolidBrush brush(Gdiplus::Color(200, 0, 122, 204)); // Semi-transparent blue    // Run the message loop
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
