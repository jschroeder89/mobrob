cmake_minimum_required(VERSION 3.12.3)
project(mobrob)

find_package(zmq REQUIRED)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(mobrob ${SOURCES})
target_link_libraries(mobrob zmq)