#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib> // For system()

class FeedDownloader {
public:
    FeedDownloader(const std::string& username, const std::string& password)
        : username(username), password(password) {}

    std::vector<std::string> getFeedUrls();

    void downloadFeed(const std::string& url, const std::string& outputFilePath);

private:
    std::string username;
    std::string password;
};

std::vector<std::string> FeedDownloader::getFeedUrls() {
    // Construct the curl command to fetch feed URLs from the Broadcastify API
    std::string apiUrl = "https://api.broadcastify.com/feeds";
    std::string command = "third_party\\curl.exe -u " + username + ":" + password +
                          " \"" + apiUrl + "\" -o feeds.json";

    // Execute the command
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to fetch feed URLs from Broadcastify API." << std::endl;
        return {};
    }

    // Parse the JSON response (assuming feeds.json contains the API response)
    std::ifstream file("feeds.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open feeds.json for reading." << std::endl;
        return {};
    }

    std::vector<std::string> feedUrls;
    std::string line;
    while (std::getline(file, line)) {
        // Parse each line to extract feed URLs (this is a placeholder; use a JSON library for real parsing)
        if (line.find("http") != std::string::npos) {
            feedUrls.push_back(line); // Replace with actual JSON parsing logic
        }
    }

    file.close();
    return feedUrls;
}

void FeedDownloader::downloadFeed(const std::string& url, const std::string& outputFilePath) {
    // Construct the curl command
    std::string command = "third_party\\curl.exe -u " + username + ":" + password +
                          " -o \"" + outputFilePath + "\" \"" + url + "\"";

    // Execute the command
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to download feed from URL: " << url << std::endl;
    }
}

// In a header file (functions.h)
void downloadFeedArchives();

void downloadFeedArchives() {
    FeedDownloader downloader("your_username", "your_password"); // Replace with actual credentials
    std::vector<std::string> feedUrls = downloader.getFeedUrls();

    if (feedUrls.empty()) {
        std::cerr << "No feed URLs found to download." << std::endl;
        return;
    }

    for (size_t i = 0; i < feedUrls.size(); ++i) {
        std::string outputFilePath = "feed_" + std::to_string(i) + ".mp3"; // Example output file naming
        downloader.downloadFeed(feedUrls[i], outputFilePath);
    }
}
