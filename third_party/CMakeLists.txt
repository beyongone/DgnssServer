CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


option (WITH_PERF_TOOL "Build with perf-tools" OFF)
option (WITH_DOC "" OFF)
option (ZMQ_BUILD_TESTS "" OFF)

#set(ZEROMQ_INCLUDE_DIR ${THIRD_PARTY}/zeromq-4.2.1/include)

add_subdirectory(zeromq-4.2.1)

add_subdirectory(zmqpp-4.1.2)

option(EVENT__BUILD_SHARED_LIBRARIES "Define if libevent should be built with shared libraries instead of archives" ON)
add_subdirectory(Libevent)

include_directories(jsoncpp-0.10.6/include)
aux_source_directory(jsoncpp-0.10.6/src/lib_json SOURCE)
add_library(jsoncpp STATIC  ${SOURCE})
set_target_properties(jsoncpp PROPERTIES DEBUG_POSTFIX "d")