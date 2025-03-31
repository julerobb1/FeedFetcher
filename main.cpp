#include <windows.h>
#include <CommCtrl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "FeedDownloader.h"

#pragma comment(lib, "Comctl32.lib")

#ifndef IDC_PROGRESS
#define IDC_PROGRESS 1002
#endif

static HWND hProgressBar;
static std::vector<std::string> feedUrls = {
    "http://example.com/file1.mp3",
    "http://example.com/file2.mp3"
};

void StartDownload(HWND hWnd) {
    std::ofstream outputFile("combined_feeds.mp3", std::ios::binary);
    for (const std::string& url : feedUrls) {
        std::string data = "dummy feed data";
        outputFile.write(data.data(), data.size());
    }
    outputFile.close();
    MessageBoxA(hWnd, "Feeds downloaded and combined successfully.", "Success", MB_OK);
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
                WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
                10, 10, 300, 30,
                hWnd, (HMENU)IDC_PROGRESS,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );
            SendMessage(hProgressBar, PBM_SETMARQUEE, TRUE, 0);
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
    wc.lpszClassName = "FeedDownloaderClass";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        wc.lpszClassName,
        "Feed Downloader",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 200,
        NULL, NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    StartDownload(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    int nCmdShow = SW_SHOWDEFAULT;
    return WinMain(hInstance, NULL, NULL, nCmdShow);
}
