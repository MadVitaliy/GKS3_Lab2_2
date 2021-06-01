cmake_minimum_required(VERSION 3.12)
project(Laba2Part2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Laba2Part2 main.cpp libs/TechProcess.h libs/Briefcase.h libs/ZipUnzip.h)