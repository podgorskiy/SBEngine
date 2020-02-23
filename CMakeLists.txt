cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0048 OLD)

project(Oquonie)

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_INSTALL OFF)
option (BUILD_EXAMPLE OFF)

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
	message("OpenAL environment missing")
endif (OPENAL_FOUND)


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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libnyquist/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources/Game)

##############################################################
# Externa libs
##############################################################
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)
add_subdirectory (libs/fsal)
add_subdirectory (libs/libnyquist)


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
# Oquonie
#####################################################################
file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h)
add_executable(Oquonie ${SOURCES})
target_link_libraries(Oquonie gl3w glfw imgui pugixml fsal lz4 zlib_static libnyquist ${OPENGL_LIBRARIES} ${OPENAL_LIBRARY})
