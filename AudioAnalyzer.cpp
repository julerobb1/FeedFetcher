#include "AudioAnalyzer.h"
#include <iostream>
#include <fstream>
#include <regex>

void AudioAnalyzer::analyze(const std::string& filePath) {
    analysisResults.clear();
    extractDTMF(filePath);
    extractCourtesyBeeps(filePath);
    extractRepeaterID(filePath);
    extractSKYWARN(filePath);
    extractRepeaterMessages(filePath); // Call the renamed function
}

void AudioAnalyzer::setInputSource(const std::string& sourceType, const std::string& source) {
    inputSourceType = sourceType;
    inputSource = source;
}

void AudioAnalyzer::setRepeaterCallsign(const std::string& callsign) {
    repeaterCallsign = callsign;
}

void AudioAnalyzer::extractDTMF(const std::string& filePath) {
    // Implement the logic to extract DTMF tones from the audio file
    std::cout << "Extracting DTMF tones from " << filePath << std::endl;
    analysisResults += "DTMF tones extracted from " + filePath + "\n";
}

void AudioAnalyzer::extractCourtesyBeeps(const std::string& filePath) {
    // Implement the logic to extract courtesy beeps from the audio file
    std::cout << "Extracting courtesy beeps from " << filePath << std::endl;
    analysisResults += "Courtesy beeps extracted from " + filePath + "\n";
}

void AudioAnalyzer::extractRepeaterID(const std::string& filePath) {
    // Implement the logic to extract repeater identification from the audio file
    std::cout << "Extracting repeater identification from " << filePath << std::endl;
    analysisResults += "Repeater identification extracted from " + filePath + "\n";
}

void AudioAnalyzer::extractSKYWARN(const std::string& filePath) {
    // Implement the logic to extract SKYWARN announcements from the audio file
    std::cout << "Extracting SKYWARN announcements from " << filePath << std::endl;
    analysisResults += "SKYWARN announcements extracted from " + filePath + "\n";
}

void AudioAnalyzer::extractRepeaterMessages(const std::string& filePath) {
    // Implement the logic to extract repeater messages from the audio file
    std::cout << "Extracting repeater messages from " << filePath << std::endl;

    // Example logic to differentiate repeater messages from normal conversation
    // This is a placeholder and should be replaced with actual logic
    std::regex systemMessagePattern("(Wrong Number Of Codes|RepeaterTimeout|Severe Thunderstorm Watch|Tornado Watch|Battery Backup|Battery Charging|RepeaterID)");
    std::smatch match;
    std::string line;
    std::ifstream inFile(filePath);

    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            if (std::regex_search(line, match, systemMessagePattern)) {
                analysisResults += "System message detected: " + match.str(0) + "\n";
            }
        }
        inFile.close();
    } else {
        std::cerr << "Failed to open file " << filePath << std::endl;
    }
}

void AudioAnalyzer::saveAnalysis(const std::string& outputFilePath) {
    std::ofstream outFile(outputFilePath);
    if (outFile.is_open()) {
        outFile << analysisResults;
        outFile.close();
        std::cout << "Analysis results saved to " << outputFilePath << std::endl;
    } else {
        std::cerr << "Failed to open file " << outputFilePath << std::endl;
    }
}
