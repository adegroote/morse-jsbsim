cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

project(jsbsim_node)

if (NOT JSBSIM_FOUND)
	find_path(JSBSIM_INCLUDE_DIR
		NAMES
			FGFDMExec.h
		PATHS
			${JSBSIM_ROOT_DIR}/include/JSBSim
			/usr/include/JSBSim
			/usr/local/include/JSBSim
	)

	find_library(JSBSIM_LIBRARY
			NAMES JSBSim
			PATHS
			${JSBSIM_ROOT_DIR}/lib
			/usr/lib/
			/usr/local/lib
	)

if (JSBSIM_INCLUDE_DIR AND JSBSIM_LIBRARY)
	set(JSBSIM_FOUND TRUE)
endif()

if (NOT JSBSIM_FOUND)
	message(FATAL_ERROR "Could not find JSBSIM")
endif()
endif()

find_package(CERTI REQUIRED)

add_definitions(-std=c++0x)

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

include_directories(${JSBSIM_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CERTI_INCLUDE_DIRS}/hla13 ${CERTI_INCLUDE_DIRS}/libhla)
add_executable(jsbsim_node main.cc jsbsim_node.cc)
target_link_libraries(jsbsim_node ${JSBSIM_LIBRARY})
target_link_libraries(jsbsim_node ${CERTI_LIBRARIES})
