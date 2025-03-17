#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <string>

class AudioAnalyzer {
public:
    void analyze(const std::string& filePath);
    void saveAnalysis(const std::string& outputFilePath);

private:
    void extractDTMF(const std::string& filePath);
    void extractCourtesyBeeps(const std::string& filePath);
    void extractRepeaterID(const std::string& filePath);
    void extractSKYWARN(const std::string& filePath);
    std::string analysisResults;
};

#endif // AUDIO_ANALYZER_H
