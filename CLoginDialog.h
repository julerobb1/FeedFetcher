#pragma once

#include <windows.h>
#include <string> // For std::string

class CLoginDialog {
public:
    CLoginDialog();
    ~CLoginDialog();

    bool ShowDialog(HINSTANCE hInstance, HWND hParent);
    std::string GetUsername() const;
    std::string GetPassword() const;

private:
    std::string username;
    std::string password;

    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};