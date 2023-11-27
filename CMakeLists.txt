cmake_minimum_required(VERSION 3.21)
project(INDiETests)

set(CMAKE_CXX_STANDARD 14)
find_package(AUSALIB REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/inc)
file(GLOB SOURCES "src/*.cpp" "src/*.c")
file(GLOB HEADERS "include/*.hh" "include/*.h")

add_executable(main main.cpp ${SOURCES} ${HEADERS})
target_link_libraries(main PRIVATE AUSA::AUSA)

add_executable(tracePlot tracePlotEx.cpp ${SOURCES} ${HEADERS})
target_link_libraries(tracePlot PRIVATE AUSA::AUSA)

add_executable(traces traceViewer.cpp)
target_link_libraries(traces PRIVATE AUSA::AUSA)