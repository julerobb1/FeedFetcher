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
    void handleFileInput(); // New method to handle file input
    void handleMicrophoneInput(); // New method to handle microphone input
    void handleDesktopPlaybackInput(); // New method to handle desktop playback input
    void extractAudioSegment(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& startTime, const std::string& duration); // New method to extract audio segment
    std::string transcribeAudio(const std::string& audioFilePath); // New method to transcribe audio
    void trimSilence(const std::string& inputFilePath, const std::string& outputFilePath); // New method to trim silence
    void playAudioFile(const std::string& filePath, int repeatCount); // New method to play audio file
    std::string analysisResults;
    std::string inputSourceType; // New member to store input source type
    std::string inputSource; // New member to store input source
    std::string repeaterCallsign; // New member to store repeater callsign
};

#endif // AUDIO_ANALYZER_H
