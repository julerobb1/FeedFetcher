#ifndef AUDIO_ANALYZER_H
#define AUDIO_ANALYZER_H

#include <string>

class AudioAnalyzer {
public:
    // Public methods
    void analyze(const std::string& filePath);
    void setInputSource(const std::string& sourceType, const std::string& source);
    void setRepeaterCallsign(const std::string& callsign);
    void saveAnalysis(const std::string& outputFilePath);
    
private:
    // Private data members
    std::string analysisResults;
    std::string inputSourceType;
    std::string inputSource;
    std::string repeaterCallsign;
    
    // Private methods
    void handleInputSource();
    void handleFileInput();
    void handleMicrophoneInput();
    void handleDesktopPlaybackInput();
    
    void extractDTMF(const std::string& filePath);
    void extractCourtesyBeeps(const std::string& filePath);
    void extractRepeaterID(const std::string& filePath);
    void extractSKYWARN(const std::string& filePath);
    void extractRepeaterMessages(const std::string& filePath);
    
    void extractAudioSegment(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& startTime, const std::string& duration);
    std::string transcribeAudio(const std::string& audioFilePath);
    void trimSilence(const std::string& inputFilePath, const std::string& outputFilePath);
    void playAudioFile(const std::string& filePath, int repeatCount);
    
    void handleDeadAir(const std::string& inputFilePath, const std::string& outputFilePath);
    void handleCallsignIdentification(const std::string& inputFilePath, const std::string& outputFilePath);
    void handleMorseCode(const std::string& inputFilePath, const std::string& outputFilePath);
};

#endif // AUDIO_ANALYZER_H