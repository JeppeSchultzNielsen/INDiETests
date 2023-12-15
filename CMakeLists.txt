cmake_minimum_required(VERSION 3.21)
project(INDiETests)

set(CMAKE_CXX_STANDARD 14)
find_package(AUSALIB REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/../xia4idsTraces/inc)
file(GLOB SOURCES "src/*.cpp" "src/*.c" "../xia4idsTraces/src/*.cpp" "../xia4idsTraces/src/*.c")
file(GLOB HEADERS "include/*.hh" "include/*.h" "../xia4idsTraces/inc/*.hh" "../xia4idsTraces/inc/*.h")
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_executable(main main.cpp ${SOURCES} ${HEADERS})
target_link_libraries(main PRIVATE AUSA::AUSA)

add_executable(tracePlot tracePlotEx.cpp ${SOURCES} ${HEADERS})
target_link_libraries(tracePlot PRIVATE AUSA::AUSA)

add_executable(traces traceViewer.cpp)
target_link_libraries(traces PRIVATE AUSA::AUSA)