#ifndef AUDIOANALYZER_H
#define AUDIOANALYZER_H

#include <string>
#include <vector>

class AudioAnalyzer {
public:
    AudioAnalyzer();
    ~AudioAnalyzer();

    void analyzeAudio(const std::string& filePath);
    void setRepeaterCallsign(const std::string& callsign);
    void setBroadcastifyCredentials(const std::string& username, const std::string& password);
    void downloadFromBroadcastify(const std::string& feedId, const std::string& startTime, const std::string& duration);
    void combineAudioFiles(const std::vector<std::string>& inputFilePaths, const std::string& outputFilePath);
    void saveAnalysisResults(const std::string& filePath);

private:
    std::string repeaterCallsign;
    std::string broadcastifyUsername;
    std::string broadcastifyPassword;
    std::string analysisResults;

    // Audio Processing Methods
    void extractDTMF(const std::string& filePath);
    void extractRepeaterMessages(const std::string& filePath);
    std::string transcribeAudio(const std::string& filePath);
    void trimSilence(const std::string& inputFilePath, const std::string& outputFilePath);
    void extractAudioSegment(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& startTime, const std::string& duration);

    // Broadcastify Methods
    bool loginToBroadcastify();
    std::string downloadAudioFromFeed(const std::string& feedId, const std::string& startTime, const std::string& duration);

    // MP3 Combination Methods
    bool combineToMP3(const std::vector<std::string>& inputFilePaths, const std::string& outputFilePath);
};

#endif
