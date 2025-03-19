#include "AudioAnalyzer.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdlib>
#include <google/cloud/speech/v1/cloud_speech_client.h>

void AudioAnalyzer::analyze(const std::string& filePath) {
    analysisResults.clear();
    handleInputSource(); // Use the new input handling method
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
    std::string outputDir = "extracted_segments/";
    std::string startTime = "00:00:00"; // Placeholder start time
    std::string duration = "00:00:10"; // Placeholder duration

    // Extract specific segments based on criteria
    extractAudioSegment(filePath, outputDir + "WrongNumberOfCodes.wav", startTime, duration);
    extractAudioSegment(filePath, outputDir + "RepeaterTimeout.wav", startTime, duration);
    extractAudioSegment(filePath, outputDir + "SevereThunderstormWatch.wav", startTime, duration);
    extractAudioSegment(filePath, outputDir + "TornadoWatch.wav", startTime, duration);
    extractAudioSegment(filePath, outputDir + "BatteryBackup.wav", startTime, duration);
    extractAudioSegment(filePath, outputDir + "BatteryCharging.wav", startTime, duration);
    extractAudioSegment(filePath, outputDir + "RepeaterID.wav", startTime, duration);

    // Trim silence from the extracted audio segments
    trimSilence(outputDir + "WrongNumberOfCodes.wav", outputDir + "WrongNumberOfCodes_trimmed.wav");
    trimSilence(outputDir + "RepeaterTimeout.wav", outputDir + "RepeaterTimeout_trimmed.wav");
    trimSilence(outputDir + "SevereThunderstormWatch.wav", outputDir + "SevereThunderstormWatch_trimmed.wav");
    trimSilence(outputDir + "TornadoWatch.wav", outputDir + "TornadoWatch_trimmed.wav");
    trimSilence(outputDir + "BatteryBackup.wav", outputDir + "BatteryBackup_trimmed.wav");
    trimSilence(outputDir + "BatteryCharging.wav", outputDir + "BatteryCharging_trimmed.wav");
    trimSilence(outputDir + "RepeaterID.wav", outputDir + "RepeaterID_trimmed.wav");

    // Transcribe the trimmed audio segments and rename based on transcription
    std::string transcription;
    transcription = transcribeAudio(outputDir + "WrongNumberOfCodes_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "WrongNumberOfCodes_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
    transcription = transcribeAudio(outputDir + "RepeaterTimeout_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "RepeaterTimeout_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
    transcription = transcribeAudio(outputDir + "SevereThunderstormWatch_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "SevereThunderstormWatch_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
    transcription = transcribeAudio(outputDir + "TornadoWatch_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "TornadoWatch_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
    transcription = transcribeAudio(outputDir + "BatteryBackup_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "BatteryBackup_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
    transcription = transcribeAudio(outputDir + "BatteryCharging_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "BatteryCharging_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
    transcription = transcribeAudio(outputDir + "RepeaterID_trimmed.wav");
    if (!transcription.empty()) {
        std::string newFileName = outputDir + transcription + ".wav";
        std::rename((outputDir + "RepeaterID_trimmed.wav").c_str(), newFileName.c_str());
        analysisResults += "Detected message: " + transcription + "\n";
    }
}

void AudioAnalyzer::handleInputSource() {
    if (inputSourceType == "file") {
        handleFileInput();
    } else if (inputSourceType == "microphone") {
        handleMicrophoneInput();
    } else if (inputSourceType == "desktop_playback") {
        handleDesktopPlaybackInput();
    } else {
        std::cerr << "Unknown input source type: " << inputSourceType << std::endl;
    }
}

void AudioAnalyzer::handleFileInput() {
    // Implement file input handling logic here
    std::cout << "Handling file input: " << inputSource << std::endl;
}

void AudioAnalyzer::handleMicrophoneInput() {
    // Implement microphone input handling logic here
    std::cout << "Handling microphone input" << std::endl;
    // Example: Use platform-specific APIs to capture audio from the microphone
    #ifdef _WIN32
    // Windows-specific microphone handling code
    #elif __linux__
    // Linux-specific microphone handling code
    #elif __APPLE__
    // macOS-specific microphone handling code
    #endif
}

void AudioAnalyzer::handleDesktopPlaybackInput() {
    // Implement desktop playback input handling logic here
    std::cout << "Handling desktop playback input" << std::endl;
    // Example: Use platform-specific APIs to capture audio from desktop playback
    #ifdef _WIN32
    // Windows-specific desktop playback handling code
    #elif __linux__
    // Linux-specific desktop playback handling code
    #elif __APPLE__
    // macOS-specific desktop playback handling code
    #endif
}

void AudioAnalyzer::extractAudioSegment(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& startTime, const std::string& duration) {
    // Use FFmpeg to extract the audio segment
    std::string command = "ffmpeg -i " + inputFilePath + " -ss " + startTime + " -t " + duration + " -c copy " + outputFilePath;
    std::cout << "Executing command: " << command << std::endl;
    std::system(command.c_str());
}

std::string AudioAnalyzer::transcribeAudio(const std::string& audioFilePath) {
    // Use Google's Speech-to-Text API to transcribe the audio
    google::cloud::speech::v1::SpeechClient client;
    google::cloud::speech::v1::RecognitionConfig config;
    config.set_encoding(google::cloud::speech::v1::RecognitionConfig::LINEAR16);
    config.set_sample_rate_hertz(16000);
    config.set_language_code("en-US");

    google::cloud::speech::v1::RecognitionAudio audio;
    std::ifstream audio_file(audioFilePath, std::ios::binary);
    audio.set_content(std::string((std::istreambuf_iterator<char>(audio_file)), std::istreambuf_iterator<char>()));

    auto response = client.Recognize(config, audio);
    if (!response) {
        std::cerr << "Error: " << response.status() << "\n";
        return "";
    }

    std::string transcription;
    for (const auto& result : response->results()) {
        for (const auto& alternative : result.alternatives()) {
            transcription += alternative.transcript() + "\n";
        }
    }
    return transcription;
}

void AudioAnalyzer::trimSilence(const std::string& inputFilePath, const std::string& outputFilePath) {
    // Use FFmpeg to trim silence from the audio file
    std::string command = "ffmpeg -i " + inputFilePath + " -af silenceremove=1:0:-50dB " + outputFilePath;
    std::cout << "Executing command: " << command << std::endl;
    std::system(command.c_str());
}

void AudioAnalyzer::playAudioFile(const std::string& filePath, int repeatCount) {
    for (int i = 0; i < repeatCount; ++i) {
        std::string command = "ffplay -nodisp -autoexit " + filePath;
        std::cout << "Playing audio file: " << filePath << std::endl;
        std::system(command.c_str());
    }
}

void AudioAnalyzer::saveAnalysis(const std::string& outputFilePath) {
    std::ofstream outFile(outputFilePath);
    if (outFile.is_open()) {
        outFile << analysisResults;
        outFile.close();
        std::cout << "Analysis results saved to " << outputFilePath << std::endl;
        playAudioFile("success.wav", 1); // Play success sound once
    } else {
        std::cerr << "Failed to open file " << outputFilePath << std::endl;
        playAudioFile("error.wav", 3); // Play error sound three times
    }
}
