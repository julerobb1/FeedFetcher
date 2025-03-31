#include "CLoginDialog.h"

LRESULT CLoginDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
    CenterWindow();
    return TRUE;
}

LRESULT CLoginDialog::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    char user[50];
    char pass[50];
    GetDlgItemTextA(IDC_USERNAME, user, 50);
    GetDlgItemTextA(IDC_PASSWORD, pass, 50);
    username = user;
    password = pass;
    EndDialog(wID);
    return 0;
}

LRESULT CLoginDialog::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    EndDialog(wID);
    return 0;
}
