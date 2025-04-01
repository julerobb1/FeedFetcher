#pragma once

#include <windows.h>
#include <string> // Ensure this header is included for std::string
#include <tchar.h> // For TCHAR and related functions
#include <commctrl.h> // For common controls (if needed for dialog functionality)
#include <atlbase.h> // Include ATL base header if required

class CLoginDialog {
public:
    CLoginDialog();
    ~CLoginDialog();

    bool ShowDialog(HINSTANCE hInstance, HWND hParent);
    std::string GetUsername() const;
    std::string GetPassword() const;

private:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    std::string username; // Ensure std::string is correctly used
    std::string password; // Ensure std::string is correctly used
};