cmake_minimum_required(VERSION 3.15)

# Project name
project(Mp3Combiner)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include necessary source files from the root directory
set(ROOT_SOURCES
    main.cpp
    app_logic.cpp
    AudioAnalyzer.cpp
    CLoginDialog.cpp
    FeedDownloader.cpp
)

# Include headers from the root directory
set(ROOT_HEADERS
    app_logic.h
    AudioAnalyzer.h
    CLoginDialog.h
    FeedDownloader.h
    resource.h
)

# Include files from the bin directory
set(BIN_FILES
    bin/curl.exe
    bin/ffmpeg.exe
    bin/ffplay.exe
    bin/ffprobe.exe
    bin/libcurl-x64.dll
    bin/curl-ca-bundle.crt
    bin/bkg-demo.jpg
)

# Include Python scripts
set(PYTHON_SCRIPTS
    python_scripts/transcriber.py
)

# Include JPEG files
set(JPEG_FILES
    assets/image1.jpeg
    assets/image2.jpeg
)

# Add executable for the project
add_executable(Mp3Combiner ${ROOT_SOURCES} ${ROOT_HEADERS})

# Add custom commands to copy runtime dependencies after Mp3Combiner is built
add_custom_command(TARGET Mp3Combiner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BIN_FILES} $<TARGET_FILE_DIR:Mp3Combiner>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PYTHON_SCRIPTS} $<TARGET_FILE_DIR:Mp3Combiner>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${JPG_FILES} $<TARGET_FILE_DIR:Mp3Combiner>
)

# Link required libraries
target_link_libraries(Mp3Combiner Comctl32)
