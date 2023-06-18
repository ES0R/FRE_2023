


cmake_minimum_required(VERSION 3.8)
project(SIFT)

set(OpenCV_DIR "/home/local/Documents/build")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(inc)  # Add the inc directory

# Server executable
file(GLOB SERVER_SOURCES "src/Server/*.cpp")
add_executable(Server ${SERVER_SOURCES})
target_link_libraries(Server ${OpenCV_LIBS} stdc++fs)

# Client executable
file(GLOB CLIENT_SOURCES "src/Client/*.cpp")
add_executable(Client ${CLIENT_SOURCES})
target_link_libraries(Client ${OpenCV_LIBS} stdc++fs)

