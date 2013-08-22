cmake_minimum_required( VERSION 2.8 )

# Project configuration
set( PROJ_NAME balls )

set( INC_DIR "${PROJECT_SOURCE_DIR}/${PROJ_NAME}/inc" )
set( SRC_DIR "${PROJECT_SOURCE_DIR}/${PROJ_NAME}/src" )

set(
    INCLUDES
    ${INC_DIR}/ballsEngine.h
    ${INC_DIR}/ballsView.h
    ${INC_DIR}/hybridCam.h
    ${INC_DIR}/includes.h
    ${INC_DIR}/math3d.h
    ${INC_DIR}/openGL.h
)    

set(
	SOURCES

	${SRC_DIR}/ballsEngine.cpp
	${SRC_DIR}/ballsView.cpp
	${SRC_DIR}/hybridCam.cpp
	${SRC_DIR}/main.cpp
	${SRC_DIR}/math3d.cpp
)

# Must be chosen before project definition
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE )
endif()

project( Balls )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )
find_package( SFML COMPONENTS system window graphics)
if(NOT SFML_FOUND)
    set(SFML_ROOT "" CACHE PATH "SFML path" FORCE)
    message(FATAL_ERROR "SFML was not found, set SFML_ROOT variable")
endif()
find_package( UniShader REQUIRED )
include_directories( ${INC_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${GLEW_INCLUDE_PATH} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${UNISHADER_INCLUDE_DIR} )


if( MSVC )
	set( STATIC_STD_LIBS false CACHE BOOL "True to link the runtime library statically, false to link them dynamically." )
endif()

# Set some useful compile flags.
if( CMAKE_COMPILER_IS_GNUCXX )
    # Use new standard
	ADD_DEFINITIONS("-Wall -Wextra -std=c++11")

	if( NOT CMAKE_CXX_FLAGS )
		set( CMAKE_CXX_FLAGS "-Wshadow -Wconversion -pedantic" CACHE STRING "C++ compiler flags" FORCE )
		set( CMAKE_C_FLAGS "-Wshadow -Wconversion -pedantic" CACHE STRING "C compiler flags" FORCE )
	endif()

	if( NOT CMAKE_CXX_FLAGS_DEBUG )
		set( CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "C++ compiler flags (debug)" FORCE )
		set( CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "C compiler flags (debug)" FORCE )
	endif()
endif()

# Static runtime linkage for Visual Studio.
if( MSVC AND STATIC_STD_LIBS )
	foreach( flag CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO )
		if( ${flag} MATCHES "/MD" )
			string( REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}" )
		endif()
	endforeach()
endif()

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

add_executable ( ${PROJ_NAME} ${INCLUDES} ${SOURCES}) 

# Link libraries
target_link_libraries( ${PROJ_NAME} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARY} ${SFML_LIBRARIES} ${UNISHADER_LIBRARY} )
