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
    downloadFeedArchives();

    return 0;
}
