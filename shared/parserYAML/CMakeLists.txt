###
### CMake settings
###
## Due to Mac OSX we need to keep compatibility with CMake 2.6
# see http://www.cmake.org/Wiki/CMake_Policies
cmake_minimum_required(VERSION 2.8)
# see http://www.cmake.org/cmake/help/cmake-2-8-docs.html#policy:CMP0012
if(POLICY CMP0012)
	cmake_policy(SET CMP0012 OLD)
endif()
# see http://www.cmake.org/cmake/help/cmake-2-8-docs.html#policy:CMP0015
if(POLICY CMP0015)
	cmake_policy(SET CMP0015 OLD)
endif()

include(CheckCXXCompilerFlag)


###
### Project settings
###
#project(YAML_CPP)

set(YAML_CPP_VERSION_MAJOR "0")
set(YAML_CPP_VERSION_MINOR "3")
set(YAML_CPP_VERSION_PATCH "0")
set(YAML_CPP_VERSION "${YAML_CPP_VERSION_MAJOR}.${YAML_CPP_VERSION_MINOR}.${YAML_CPP_VERSION_PATCH}")

#enable_testing()


###
### Project options
###
## Project stuff
#option(YAML_CPP_BUILD_TOOLS "Enable testing and parse tools" ON)
#option(YAML_CPP_BUILD_CONTRIB "Enable contrib stuff in library" ON)

## Build options
# --> General
# see http://www.cmake.org/cmake/help/cmake2.6docs.html#variable:BUILD_SHARED_LIBS
#     http://www.cmake.org/cmake/help/cmake2.6docs.html#command:add_library
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)

# --> Apple
#option(APPLE_UNIVERSAL_BIN "Apple: Build universal binary" OFF)

# --> Microsoft Visual C++
# see http://msdn.microsoft.com/en-us/library/aa278396(v=VS.60).aspx
#     http://msdn.microsoft.com/en-us/library/2kzt1wy3(v=VS.71).aspx
#option(MSVC_SHARED_RT "MSVC: Build with shared runtime libs (/MD)" ON)
#option(MSVC_STHREADED_RT "MSVC: Build with single-threaded static runtime libs (/ML until VS .NET 2003)" OFF)

###
### Sources, headers, directories and libs
###
set(header_directory "include/yaml-cpp/")

file(GLOB sources "src/[a-zA-Z]*.cpp")
file(GLOB public_headers "include/yaml-cpp/[a-zA-Z]*.h")
file(GLOB private_headers "src/[a-zA-Z]*.h")

if(YAML_CPP_BUILD_CONTRIB)
	file(GLOB contrib_sources "src/contrib/[a-zA-Z]*.cpp")
	file(GLOB contrib_public_headers "include/yaml-cpp/contrib/[a-zA-Z]*.h")
	file(GLOB contrib_private_headers "src/contrib/[a-zA-Z]*.h")
else()
	add_definitions(-DYAML_CPP_NO_CONTRIB)
endif()

if(VERBOSE)
	message(STATUS "sources: ${sources}")
	message(STATUS "public_headers: ${public_headers}")
	message(STATUS "private_headers: ${private_headers}")
	message(STATUS "contrib_sources: ${contrib_sources}")
	message(STATUS "contrib_public_headers: ${contrib_public_headers}")
	message(STATUS "contrib_private_headers: ${contrib_private_headers}")
endif()

include_directories(${YAML_CPP_SOURCE_DIR}/include/)
include_directories(${YAML_CPP_SOURCE_DIR}/src)
include_directories(include/)

###
### General compilation settings
###
if(BUILD_SHARED_LIBS)
	set(LABEL_SUFFIX "shared")
else()
	set(LABEL_SUFFIX "static")
endif()

# GCC specialities
if(CMAKE_COMPILER_IS_GNUCXX)
	### General stuff
	
	### Project stuff
	if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Release)
	endif()
	#
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
	#
	set(GCC_EXTRA_OPTIONS "")
	#
	set(FLAG_TESTED "-Wextra")
	check_cxx_compiler_flag(${FLAG_TESTED} FLAG_WEXTRA)
	if(FLAG_WEXTRA)
		set(GCC_EXTRA_OPTIONS "${GCC_EXTRA_OPTIONS} ${FLAG_TESTED}")
	endif()
	#
	set(CMAKE_CXX_FLAGS "-Wall ${GCC_EXTRA_OPTIONS} -pedantic -Wno-long-long ${CMAKE_CXX_FLAGS}")
	#
	add_custom_target(debuggable $(MAKE) clean
		COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
		COMMENT "Adjusting settings for debug compilation"
		VERBATIM)
	add_custom_target(releasable $(MAKE) clean
		COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
		COMMENT "Adjusting settings for release compilation"
		VERBATIM)
endif()



###
### Library
###
add_library(yaml-cpp
	${sources}
	${public_headers}
	${private_headers}
	${contrib_sources}
	${contrib_public_headers}
	${contrib_private_headers}
)

set_target_properties(yaml-cpp PROPERTIES
	VERSION "${YAML_CPP_VERSION}"
	SOVERSION "${YAML_CPP_VERSION_MAJOR}.${YAML_CPP_VERSION_MINOR}"
	PROJECT_LABEL "yaml-cpp ${LABEL_SUFFIX}"
)


if(UNIX)
	#set(PC_FILE ${CMAKE_BINARY_DIR}/yaml-cpp.pc)
	#configure_file("yaml-cpp.pc.cmake" ${PC_FILE} @ONLY)
	#install(FILES ${PC_FILE} DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()


