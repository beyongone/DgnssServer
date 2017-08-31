CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${COMN_DIR}/include)

if (MSVC)
	include_directories(${THIRD_PARTY}/stdint)
endif()

if (WIN32)
	include_directories(${THIRD_PARTY}/Libevent/win32)
else()
	include_directories(${THIRD_PARTY}/Libevent/include)
endif()

include_directories(${ZEROMQ_DIR}/include)
include_directories(${ZMQPP_DIR}/src)
include_directories(../AgnssCommon)

link_directories(${PROJECT_ROOT}/lib)
link_directories(${COMN_DIR}/lib)
link_directories(${THIRD_PARTY}/Libevent/lib)


aux_source_directory(. SOURCES)
aux_source_directory(../AgnssCommon SOURCES)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:.")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(AgnssCapture ${SOURCES})

if (MSVC)
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_link_libraries(AgnssCapture
	${LIB_PLATFORM}
	debug comnd optimized comn
	event
	zmqpp-static
	libzmq
	)

	
install(TARGETS AgnssCapture DESTINATION .)

install(FILES ${PROJECT_SOURCE_DIR}/setup/AgnssCapture.ini DESTINATION .)


