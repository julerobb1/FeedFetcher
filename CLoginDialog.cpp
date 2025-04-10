#include "CLoginDialog.h"
#include "resource.h"
#include <windowsx.h> // For dialog control macros

CLoginDialog::CLoginDialog() : username(""), password("") {}

CLoginDialog::~CLoginDialog() {}

bool CLoginDialog::ShowDialog(HINSTANCE hInstance, HWND hParent) {
    return DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_LOGIN_DIALOG), hParent, DialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}

std::string CLoginDialog::GetUsername() const {
    return username;
}

std::string CLoginDialog::GetPassword() const {
    return password;
}

INT_PTR CALLBACK CLoginDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    CLoginDialog* pThis = nullptr;

    if (uMsg == WM_INITDIALOG) {
        pThis = reinterpret_cast<CLoginDialog*>(lParam);
        SetWindowLongPtr(hwndDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        return TRUE;
    }

    pThis = reinterpret_cast<CLoginDialog*>(GetWindowLongPtr(hwndDlg, GWLP_USERDATA));
    if (!pThis) return FALSE;

    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            char buffer[256];
            GetDlgItemText(hwndDlg, IDC_USERNAME, buffer, sizeof(buffer));
            pThis->username = buffer;

            GetDlgItemText(hwndDlg, IDC_PASSWORD, buffer, sizeof(buffer));
            pThis->password = buffer;

            EndDialog(hwndDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }

    return FALSE;
}
