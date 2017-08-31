CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(testComn comn.cpp)

target_link_libraries(testComn debug comnd optimized comn)

