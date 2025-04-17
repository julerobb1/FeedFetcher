cmake_minimum_required(VERSION 3.15)

# Project name
project(FeedFetcher)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable graphical mode
option(GRAPHICAL_MODE "Enable graphical mode" ON)

# Define build number (can be passed via command line or default to 1)
if(NOT DEFINED BUILD_NUMBER)
    set(BUILD_NUMBER 1)
endif()

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

# Define source and destination directories with trailing slashes
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/)
set(RELEASE_DIR ${CMAKE_BINARY_DIR}/Release/)

# Include files from the bin directory
set(BIN_FILES
    ${BIN_DIR}curl.exe
    ${BIN_DIR}ffmpeg.exe
    ${BIN_DIR}ffplay.exe
    ${BIN_DIR}ffprobe.exe
    ${BIN_DIR}libcurl-x64.dll
    ${BIN_DIR}curl-ca-bundle.crt
    ${BIN_DIR}bkg-demo.jpeg
)

# Include Python scripts
set(PYTHON_SCRIPTS
    ${CMAKE_SOURCE_DIR}/python_scripts/transcriber.py
)

# Set executable name with build number
set(EXECUTABLE_NAME "FeedFetcher_${BUILD_NUMBER}")

# Ensure the output directory is set correctly
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add executable for the project
add_executable(${EXECUTABLE_NAME} ${ROOT_SOURCES})

# Pass GRAPHICAL_MODE definition to the compiler if enabled
if(GRAPHICAL_MODE)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE GRAPHICAL_MODE)
    # Set the subsystem to WINDOWS for graphical mode
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
else()
    # Set the subsystem to CONSOLE for console mode
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

# Add custom commands to copy runtime dependencies after FeedFetcher is built
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BIN_FILES} $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PYTHON_SCRIPTS} $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
)

# Link required libraries
target_link_libraries(${EXECUTABLE_NAME} Comctl32)

# Ensure required files are copied to the Release directory
file(COPY ${BIN_FILES} DESTINATION ${RELEASE_DIR})
file(COPY ${PYTHON_SCRIPTS} DESTINATION ${RELEASE_DIR})

# Ensure the transcribe.py script is copied
file(COPY ${CMAKE_SOURCE_DIR}/transcribe.py DESTINATION ${RELEASE_DIR})
