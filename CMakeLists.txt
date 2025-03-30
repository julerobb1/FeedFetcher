cmake_minimum_required(VERSION 3.10)
project(Mp3Combiner VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(Mp3Combiner 
    main.cpp
    AudioAnalyzer.cpp
    # Add any other source files here
)

# Include directories
target_include_directories(Mp3Combiner PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# If you're using external libraries, add them like this:
# find_package(ExternalLib REQUIRED)
# target_link_libraries(Mp3Combiner PRIVATE ExternalLib)

# Copy resource files to build directory
# add_custom_command(TARGET Mp3Combiner POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy 
#     ${CMAKE_SOURCE_DIR}/success.wav
#     ${CMAKE_SOURCE_DIR}/error.wav
#     $<TARGET_FILE_DIR:Mp3Combiner>
# )
