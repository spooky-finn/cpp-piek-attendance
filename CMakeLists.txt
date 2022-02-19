# version 3.11 or later of CMake needed later for installing GoogleTest
# so let's require it now.
cmake_minimum_required(VERSION 3.11s-3.18)

project(piekAttendance VERSION 0.1)

add_subdirectory(json)
add_subdirectory(src)