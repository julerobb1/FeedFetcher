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
    ${CMAKE_SOURCE_DIR}/bin/curl.exe
    ${CMAKE_SOURCE_DIR}/bin/ffmpeg.exe
    ${CMAKE_SOURCE_DIR}/bin/ffplay.exe
    ${CMAKE_SOURCE_DIR}/bin/ffprobe.exe
    ${CMAKE_SOURCE_DIR}/bin/libcurl-x64.dll
    ${CMAKE_SOURCE_DIR}/bin/curl-ca-bundle.crt
    ${CMAKE_SOURCE_DIR}/bin/bkg-demo.jpg
)

# Include Python scripts
set(PYTHON_SCRIPTS
    ${CMAKE_SOURCE_DIR}/python_scripts/transcriber.py
)

# Include JPEG files
set(JPEG_FILES
    ${CMAKE_SOURCE_DIR}/assets/image1.jpeg
    ${CMAKE_SOURCE_DIR}/assets/image2.jpeg
)

# Add executable for the project
add_executable(Mp3Combiner ${ROOT_SOURCES} ${ROOT_HEADERS})

# Add custom commands to copy runtime dependencies after Mp3Combiner is built
add_custom_command(TARGET Mp3Combiner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BIN_FILES} $<TARGET_FILE_DIR:Mp3Combiner>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PYTHON_SCRIPTS} $<TARGET_FILE_DIR:Mp3Combiner>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${JPEG_FILES} $<TARGET_FILE_DIR:Mp3Combiner>
)

# Link required libraries
target_link_libraries(Mp3Combiner Comctl32)
