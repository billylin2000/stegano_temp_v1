cmake_minimum_required(VERSION 2.8)
project( PVD )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( pvd main.cpp )
target_link_libraries( pvd ${OpenCV_LIBS} )
