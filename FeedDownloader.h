#pragma once

#include <string>
#include <vector>

class FeedDownloader {
public:
    FeedDownloader(const std::string& username, const std::string& password);
    std::vector<std::string> getFeedUrls();
    std::vector<char> downloadFeed(const std::string& url);

private:
    std::string username;
    std::string password;
};
