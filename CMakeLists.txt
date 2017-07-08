cmake_minimum_required(VERSION 3.7)
project(Reverb)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# compiler options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weffc++ -Werror -pedantic")

# sndfile
find_package(SNDFILE REQUIRED)
include_directories(${SNDFILE_INCLUDE_DIR})

# main executable
include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(ReverbMain ${SOURCES})

# link libraries
target_link_libraries(ReverbMain ${SNDFILE_LIBRARIES})
