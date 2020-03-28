cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0079 NEW)

configure_file(albido.pvr albido.pvr COPYONLY)
configure_file(LeePerrySmith.obj LeePerrySmith.obj COPYONLY)

project(Oquonie)

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_INSTALL OFF)
option (BUILD_EXAMPLE OFF)
option (BUILD_SHARED_LIBS OFF)

option(TESTS "Build tests" ON)

option (USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
find_package(OpenAL)

if (OPENGL_FOUND)
	message("OpenGL Correctly Found")
	include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
	message("OpenGL environment missing")
endif (OPENGL_FOUND)

if (OPENAL_FOUND)
	message("OpenAL Correctly Found")
	include_directories(${OPENAL_INCLUDE_DIR})
else (OPENAL_FOUND)
	if(MSVC)
	set(OPENAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/openal/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/openal/libs/Win64/)
	set(OPENAL_LIBRARY OpenAL32)
	include_directories(${OPENAL_INCLUDE_DIR})
	else()
		message("OpenAL environment missing")
	endif()
endif (OPENAL_FOUND)


FUNCTION(PREPEND var prefix)
    SET(listVar "")
    FOREACH(f ${ARGN})
        LIST(APPEND listVar "${prefix}/${f}")
    ENDFOREACH(f)
    SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)


if(MSVC)
else()
	set(DEBUG_OPTIONS -g -Wall)
	set(RELEASE_OPTIONS -Ofast -fPIC -funsafe-math-optimizations -fno-strict-aliasing -fno-rtti -ffast-math -flto -msse2 -msse3 -msse4)
endif()

add_compile_options(
        "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>"
        "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")

##############################################################
# Generating OpenGL profile
##############################################################
message("Executing gl3w_gen.py")
execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/libs/gl3w WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

##############################################################
# Includes
##############################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/gl3w/include)
include_directories(${CMAKE_BINARY_DIR}/libs/gl3w/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/fsal/sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/lz4/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/minimp3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libogg/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libvorbis/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources/Game)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/doctest/doctest)


##############################################################
# Externa libs
##############################################################
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)
add_subdirectory (libs/fsal)


#####################################################################
# imgui
#####################################################################
set(IMGUI_DIR libs/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h )
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp)


#####################################################################
# lz4
#####################################################################
set(LZ4_DIR libs/lz4/lib/)
set(SOURCES_LZ4 ${LZ4_DIR}lz4.c ${LZ4_DIR}lz4hc.c ${LZ4_DIR}lz4.h ${LZ4_DIR}lz4hc.h ${LZ4_DIR}xxhash.c ${LZ4_DIR}xxhash.h)
add_library(lz4 ${SOURCES_LZ4})
#####################################################################


#####################################################################
# zlib
#####################################################################
file(GLOB ZLIB_SOURCES libs/zlib/*.c libs/zlib/*.h)
add_library(zlib_static STATIC ${ZLIB_SOURCES})
#####################################################################


#####################################################################
# pugixml
#####################################################################
file(GLOB PUGIXML_SOURCES libs/pugixml/src/*.cpp libs/pugixml/src/*.hpp)
add_library(pugixml ${PUGIXML_SOURCES})
#####################################################################

#####################################################################
# libvorbis
#####################################################################
set(VORBIS_SOURCES analysis.c bitrate.c block.c codebook.c envelope.c floor0.c floor1.c info.c lpc.c lsp.c mapping0.c psy.c registry.c res0.c sharedbook.c smallft.c synthesis.c vorbisenc.c vorbisfile.c window.c mdct.c)
PREPEND(VORBIS_SOURCES libs/libvorbis/src ${VORBIS_SOURCES})
add_library(vorbis ${VORBIS_SOURCES})
target_include_directories(vorbis PRIVATE libs/libvorbis/src)
#####################################################################

#####################################################################
# libogg
#####################################################################
set(OGG_SOURCES bitwise.c framing.c)
PREPEND(OGG_SOURCES libs/libogg/src ${OGG_SOURCES})
add_library(ogg ${OGG_SOURCES})
#####################################################################


#####################################################################
# Oquonie
#####################################################################
file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h)
if (TESTS)
	file(GLOB_RECURSE TESTS tests/*.cpp)
endif()

add_executable(Oquonie ${SOURCES} ${TESTS})
target_link_libraries(Oquonie gl3w glfw imgui pugixml fsal lz4 zlib_static vorbis ogg ${OPENGL_LIBRARIES} ${OPENAL_LIBRARY})


if(MSVC)
else()
	target_link_libraries(Oquonie stdc++fs)
	target_compile_options(Oquonie PRIVATE -lstdc++fs -Wno-switch)
	target_link_options(Oquonie PRIVATE -static-libstdc++)
endif()

