#include "FeedDownloader.h"
#include <curl/curl.h>

FeedDownloader::FeedDownloader(const std::string& username, const std::string& password)
    : username(username), password(password) {}

std::vector<std::string> FeedDownloader::getFeedUrls() {
    // Implement the logic to fetch feed URLs using the Broadcastify API
    // This is a placeholder implementation
    return { "http://example.com/feed1.mp3", "http://example.com/feed2.mp3" };
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::vector<char>*)userp)->insert(((std::vector<char>*)userp)->end(), (char*)contents, (char*)contents + size * nmemb);
    return size * nmemb;
}

std::vector<char> FeedDownloader::downloadFeed(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::vector<char> data;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return data;
}
