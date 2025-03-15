#include <atlbase.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "CLoginDialog.h"
#include "FeedDownloader.h"

class CMainWindow : public CWindowImpl<CMainWindow> {
public:
    DECLARE_WND_CLASS(NULL)

    BEGIN_MSG_MAP(CMainWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
        progressBar.Create(m_hWnd, NULL, NULL, WS_CHILD | WS_VISIBLE | PBS_MARQUEE, 0, IDC_PROGRESS);
        progressBar.SetMarquee(TRUE, 0);
        return 0;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
        PostQuitMessage(0);
        return 0;
    }

    void StartDownload() {
        CLoginDialog dlg;
        if (dlg.DoModal() == IDOK) {
            std::string username = dlg.username;
            std::string password = dlg.password;

            FeedDownloader downloader(username, password);
            std::vector<std::string> feedUrls = downloader.getFeedUrls();

            std::ofstream outputFile("combined.mp3", std::ios::binary);
            for (const std::string& url : feedUrls) {
                std::vector<char> data = downloader.downloadFeed(url);
                outputFile.write(data.data(), data.size());
            }
            outputFile.close();

            MessageBoxA(m_hWnd, "Feeds downloaded and combined successfully.", "Success", MB_OK);
        }
    }

private:
    CProgressBarCtrl progressBar;
};

CMainWindow theApp;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CMainWindow wnd;
    MSG msg;
    wnd.Create(NULL, CWindow::rcDefault, "Feed Downloader", WS_OVERLAPPEDWINDOW);
    wnd.ShowWindow(nCmdShow);
    wnd.UpdateWindow();
    wnd.StartDownload();

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
