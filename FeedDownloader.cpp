#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h> // Ensure libcurl is installed and properly linked in your build system
// Make sure to link libcurl in your build system (e.g., using vcpkg or manually adding the library path)

class FeedDownloader {
public:
    FeedDownloader(const std::string& username, const std::string& password)
        : username(username), password(password) {}

    std::vector<std::string> getFeedUrls();

    void downloadFeed(const std::string& url, const std::string& outputFilePath);

private:
    static size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream);

    std::string username;
    std::string password;
};

std::vector<std::string> FeedDownloader::getFeedUrls() {
    // Implement the logic to fetch feed URLs using the Broadcastify API
    // This is a placeholder implementation
    return { "http://example.com/feed1.mp3", "http://example.com/feed2.mp3" };
}

void FeedDownloader::downloadFeed(const std::string& url, const std::string& outputFilePath) {
    CURL* curl;
    FILE* file;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        file = fopen(outputFilePath.c_str(), "wb");
        if (!file) {
            std::cerr << "Failed to open file: " << outputFilePath << std::endl;
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        }

        fclose(file);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }
}

size_t FeedDownloader::writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}
