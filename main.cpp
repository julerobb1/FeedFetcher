#include "AudioAnalyzer.h"
#include <iostream>
#include <string>

int main() {
    AudioAnalyzer analyzer;
    analyzer.analyzeAudio("test.mp3");
    analyzer.setRepeaterCallsign("W1AW");
    analyzer.setBroadcastifyCredentials("username", "password");
    analyzer.downloadFromBroadcastify("12345", "00:00", "01:00");
    return 0;
}
