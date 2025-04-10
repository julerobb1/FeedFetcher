cmake_minimum_required(VERSION 3.15)

# Project name
project(Mp3Combiner)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
set(SOURCES
    app_logic.cpp
    feed_fetcher.cpp
    main.cpp
    # Add other source files here
)
add_executable(Mp3Combiner ${SOURCES})

# Link required libraries
target_link_libraries(Mp3Combiner Comctl32)
