#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <string>

class AudioAnalyzer {
public:
    void analyze(const std::string& filePath);
    void saveAnalysis(const std::string& outputFilePath);
    void setInputSource(const std::string& sourceType, const std::string& source); // New method to set input source
    void setRepeaterCallsign(const std::string& callsign); // New method to set repeater callsign

private:
    void extractDTMF(const std::string& filePath);
    void extractCourtesyBeeps(const std::string& filePath);
    void extractRepeaterID(const std::string& filePath);
    void extractSKYWARN(const std::string& filePath);
    void extractRepeaterMessages(const std::string& filePath); // Renamed function
    std::string analysisResults;
    std::string inputSourceType; // New member to store input source type
    std::string inputSource; // New member to store input source
    std::string repeaterCallsign; // New member to store repeater callsign
};

#endif // AUDIO_ANALYZER_H
