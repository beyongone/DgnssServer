CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

aux_source_directory(. SOURCES)
add_executable(demo ${SOURCES})

target_link_libraries(demo debug comnd optimized comn)