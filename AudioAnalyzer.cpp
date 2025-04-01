#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
class AudioAnalyzer {
public:
    void analyze(const std::vector<std::string>& filePaths, const std::string& outputFilePath);
    void trimSilence(const std::string& inputFilePath, const std::string& outputFilePath);
    void mergeFiles(const std::vector<std::string>& inputFiles, const std::string& outputFilePath);
    void extractRepeaterMessages(const std::string& inputFilePath, const std::string& outputDir);
    void extractCourtesyBeeps(const std::string& inputFilePath, const std::string& outputDir);
    void transcribeAudio(const std::string& inputFilePath, const std::string& outputFilePath);

private:
    void executeCommand(const std::string& command);
};

void AudioAnalyzer::analyze(const std::vector<std::string>& filePaths, const std::string& outputFilePath) {
    std::vector<std::string> processedFiles;

    for (const auto& filePath : filePaths) {
        std::string outputDir = "processed_segments/";
        std::filesystem::create_directory(outputDir);

        extractCourtesyBeeps(filePath, outputDir);
        extractRepeaterMessages(filePath, outputDir);

        std::string trimmedFile = filePath + "_trimmed.wav";
        trimSilence(filePath, trimmedFile);
        processedFiles.push_back(trimmedFile);
    }

    mergeFiles(processedFiles, outputFilePath);
    std::cout << "Analysis complete. Output saved to: " << outputFilePath << std::endl;
}

void AudioAnalyzer::trimSilence(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Trimming silence from: " << inputFilePath << std::endl;
    std::string command = "ffmpeg -i \"" + inputFilePath + "\" -af silenceremove=1:0:-50dB \"" + outputFilePath + "\"";
    executeCommand(command);
}

void AudioAnalyzer::mergeFiles(const std::vector<std::string>& inputFiles, const std::string& outputFilePath) {
    std::cout << "Merging files into: " << outputFilePath << std::endl;

    // Create a temporary file list for FFmpeg
    std::string fileList = "file_list.txt";
    std::ofstream listFile(fileList);
    for (const auto& file : inputFiles) {
        listFile << "file '" << file << "'\n";
    }
    listFile.close();

    // Use FFmpeg to merge files
    std::string command = "ffmpeg -f concat -safe 0 -i " + fileList + " -c copy \"" + outputFilePath + "\"";
    executeCommand(command);

    // Clean up temporary file
    std::filesystem::remove(fileList);
}

void AudioAnalyzer::extractRepeaterMessages(const std::string& inputFilePath, const std::string& outputDir) {
    std::cout << "Extracting repeater messages from: " << inputFilePath << std::endl;

    // Placeholder logic for extracting repeater messages
    std::string startTime = "00:00:00"; // Example start time
    std::string duration = "00:00:10"; // Example duration

    std::vector<std::string> messageTypes = {
        "WrongNumberOfCodes", "RepeaterTimeout", "SevereThunderstormWatch",
        "TornadoWatch", "BatteryBackup", "BatteryCharging", "RepeaterID"
    };

    for (const auto& messageType : messageTypes) {
        std::string outputFilePath = outputDir + messageType + ".wav";
        std::string command = "ffmpeg -i \"" + inputFilePath + "\" -ss " + startTime + " -t " + duration + " -c copy \"" + outputFilePath + "\"";
        executeCommand(command);
    }
}

void AudioAnalyzer::extractCourtesyBeeps(const std::string& inputFilePath, const std::string& outputDir) {
    std::cout << "Extracting courtesy beeps from: " << inputFilePath << std::endl;

    // Placeholder logic for extracting courtesy beeps
    std::string outputFilePath = outputDir + "CourtesyBeeps.wav";
    std::string command = "ffmpeg -i \"" + inputFilePath + "\" -af \"highpass=f=1000,lowpass=f=2000\" \"" + outputFilePath + "\"";
    executeCommand(command);
}

void AudioAnalyzer::transcribeAudio(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Setting up dependencies for transcription..." << std::endl;

    // Run the setup script to ensure dependencies are installed
    std::string setupCommand = "python setup.py";
    int setupResult = std::system(setupCommand.c_str());
    if (setupResult != 0) {
        std::cerr << "Failed to set up dependencies. Aborting transcription." << std::endl;
        return;
    }

    std::cout << "Transcribing audio from: " << inputFilePath << std::endl;

    // Call the transcription script
    std::string command = "python transcribe.py \"" + inputFilePath + "\" \"" + outputFilePath + "\"";
    executeCommand(command);
}

void AudioAnalyzer::executeCommand(const std::string& command) {
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << std::endl;
    }
}
