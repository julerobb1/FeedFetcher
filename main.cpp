#include "AudioAnalyzer.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "MP3 Combiner Starting..." << std::endl;
    
    AudioAnalyzer analyzer;
    
    // Set default values
    analyzer.setInputSource("file", "input.mp3");
    analyzer.setRepeaterCallsign("N0CALL");
    
    // Process command line arguments
    std::string inputFile = (argc > 1) ? argv[1] : "input.mp3";
    
    // Run analysis
    analyzer.analyze(inputFile);
    
    // Save results
    analyzer.saveAnalysis("analysis_results.txt");
    
    std::cout << "Analysis complete!" << std::endl;
    return 0;
}
