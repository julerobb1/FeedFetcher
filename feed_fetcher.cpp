#include <iostream>
#include <cstdlib>
#include <string>

// Logs in to Broadcastify and saves cookies for subsequent requests
void loginToBroadcastify(const std::string& username, const std::string& password) {
    std::string curlPath = "third_party/curl/";
#ifdef _WIN64
    curlPath += "x64/curl.exe";
#else
    curlPath += "x86/curl.exe";
#endif

    std::string cookieFile = "cookies.txt";
    std::string loginUrl = "https://www.broadcastify.com/login";
    std::string command = curlPath + " -c " + cookieFile + " -d \"username=" + username + "&password=" + password + "\" " + loginUrl;

    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Login successful." << std::endl;
    } else {
        std::cerr << "Failed to log in. Error code: " << result << std::endl;
    }
}

// Downloads feed archives using the Feed Owner API
void downloadFeedArchives() {
    std::string curlPath = "third_party/curl/";
#ifdef _WIN64
    curlPath += "x64/curl.exe";
#else
    curlPath += "x86/curl.exe";
#endif

    std::string feedUrl = "https://example.com/user/feeds/today";
    std::string outputFile = "archives/feed_archive.zip";
    std::string command = curlPath + " -o " + outputFile + " " + feedUrl;

    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Feed archives downloaded successfully." << std::endl;
    } else {
        std::cerr << "Failed to download feed archives. Error code: " << result << std::endl;
    }
}

// Entry point for the application
int main() {
    std::string username = "your_username"; // Replace with actual username
    std::string password = "your_password"; // Replace with actual password

    loginToBroadcastify(username, password);
    downloadFeedArchives();

    return 0;
}
