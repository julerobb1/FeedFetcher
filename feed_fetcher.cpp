#include <iostream>
#include <cstdlib>
#include <string>
#include "app_logic.h"

// This file can now focus on other logic specific to feed fetching
// without duplicating shared functions.

// Entry point for the application
int main() {
    std::string username = "your_username"; // Replace with actual username
    std::string password = "your_password"; // Replace with actual password

    loginToBroadcastify(username, password);

    HWND hWnd = nullptr; // Replace with the actual HWND if available
    HWND hProgressBar = nullptr; // Replace with the actual HWND if available
    std::wstring feedUrl = L"https://www.broadcastify.com/archives/feed/39972"; // Example feed URL

    // Pass the required arguments to downloadFeedArchives
    downloadFeedArchives(hWnd, hProgressBar, feedUrl);

    return 0;
}
