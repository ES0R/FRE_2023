cmake_minimum_required(VERSION 3.0)
project( SIFT )
find_package( OpenCV REQUIRED )

#
include_directories( ${OpenCV_INCLUDE_DIRS} )

#
file(GLOB SOURCES "src/*.cpp")

#
add_executable( SIFT ${SOURCES})
target_link_libraries( SIFT ${OpenCV_LIBS} )
