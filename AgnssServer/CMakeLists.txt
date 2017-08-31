CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${COMN_DIR}/include)

if (MSVC)
	include_directories(${THIRD_PARTY}/stdint)
endif()

include_directories(${ZEROMQ_DIR}/include)
include_directories(${ZMQPP_DIR}/src)
include_directories(../AgnssCommon)
include_directories(${THIRD_PARTY}/jsoncpp-0.10.6/include)

link_directories(${PROJECT_ROOT}/lib)
link_directories(${COMN_DIR}/lib)


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


add_executable(AgnssServer ${SOURCES})

if (MSVC)
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_link_libraries(AgnssServer
	${LIB_PLATFORM}
	debug comnd optimized comn
	zmqpp-static
	libzmq
	debug jsoncppd optimized jsoncpp
	)



install(TARGETS AgnssServer DESTINATION .)

install(FILES ${PROJECT_SOURCE_DIR}/setup/AgnssServer.ini DESTINATION .)

if(WIN32)
	install(FILES ${PROJECT_SOURCE_DIR}/bin/event.dll DESTINATION .)
	install(FILES ${PROJECT_SOURCE_DIR}/bin/libzmq-v140-mt-4_2_1.dll DESTINATION .)
else()
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/third_party/Libevent/lib/ DESTINATION . FILES_MATCHING PATTERN "libevent*")
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/lib/ DESTINATION . FILES_MATCHING PATTERN "libzmq*")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "AgnssServer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AgnssServer")
set(CPACK_PACKAGE_VENDOR "www.100wits.com")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

SET(CPACK_GENERATOR "ZIP")

set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../dist") 
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}") 

include(CPack)