cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0079 NEW)

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)
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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/scriberlib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/yaml-cpp/include)


##############################################################
# Externa libs
##############################################################
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)
add_subdirectory (libs/fsal)
add_subdirectory (libs/scriberlib)
add_subdirectory (libs/yaml-cpp)


#####################################################################
# bx
#####################################################################
set(BX_DIR libs/bx)
set(BX_3DPARTY ${BX_DIR}/3rdparty)
set(BX_INCLUDE ${BX_DIR}/include)
include_directories(${BX_INCLUDE})
include_directories(${BX_3DPARTY})

if(MSVC)
include_directories(${BX_INCLUDE}/compat/msvc)
endif()
file(GLOB BX_SOURCES ${BX_DIR}/src/*.cpp ${BX_INCLUDE}/*.h)
list(REMOVE_ITEM BX_SOURCES BX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${BX_DIR}/src/amalgamated.cpp)
add_library(bx ${BX_SOURCES})
#####################################################################

#####################################################################
# bimg
#####################################################################
set(BIMG_DIR libs/bimg)
set(BIMG_INCLUDE ${BIMG_DIR}/include)
include_directories(${BGFX_DIR}/3rdparty)
include_directories(${BIMG_INCLUDE})
include_directories(${BIMG_DIR}/3rdparty)
include_directories(${BIMG_DIR}/3rdparty/iqa/include)
include_directories(${BIMG_DIR}/3rdparty/nvtt)
include_directories(${BIMG_DIR}/3rdparty/astc)
include_directories(${BIMG_DIR}/3rdparty/astc-codec)
include_directories(${BIMG_DIR}/3rdparty/astc-codec/include)
file(GLOB BIMG_SOURCES ${BIMG_DIR}/src/image.* ${BIMG_DIR}/src/image_gnf.cpp ${BIMG_DIR}/src/decoder/astc_file.* ${BIMG_INCLUDE}/*.h)

set(ASTC_CODEC_DIR ${BIMG_DIR}/3rdparty/astc-codec/src/decoder)
file(GLOB ASTC_3RDPARTY_SOURCES
		${ASTC_CODEC_DIR}/astc_file.*
		${ASTC_CODEC_DIR}/codec.*
		${ASTC_CODEC_DIR}/endpoint_codec.*
		${ASTC_CODEC_DIR}/footprint.*
		${ASTC_CODEC_DIR}/integer_sequence_codec.*
		${ASTC_CODEC_DIR}/intermediate_astc_block.*
		${ASTC_CODEC_DIR}/logical_astc_block.*
		${ASTC_CODEC_DIR}/partition.*
		${ASTC_CODEC_DIR}/physical_astc_block.*
		${ASTC_CODEC_DIR}/quantization.*
		${ASTC_CODEC_DIR}/weight_infill.*
		)

add_library(bimg ${BIMG_SOURCES} ${ASTC_3RDPARTY_SOURCES})
target_link_libraries(bimg bx)
#####################################################################

#####################################################################
# bgfx
#####################################################################
set(BGFX_DIR libs/bgfx)
set(BGFX_INCLUDE ${BGFX_DIR}/include)
include_directories(${BGFX_DIR}/tools/shaderc)
include_directories(${BGFX_DIR}/3rdparty)
include_directories(${BGFX_DIR}/3rdparty/khronos)
include_directories(${BGFX_DIR}/3rdparty/spirv-cross)
include_directories(${BGFX_DIR}/3rdparty/dxsdk/include)
include_directories(libs/imgui)
include_directories(${BGFX_INCLUDE})
if(MSVC)
include_directories(${BGFX_INCLUDE}/compat/msvc)
endif()
file(GLOB BGFX_SOURCES
	${BGFX_DIR}/src/*.cpp
	${BGFX_DIR}/src/*.h
	${BGFX_INCLUDE}/*.h
	${BGFX_DIR}/src/*.inl
	)
list(REMOVE_ITEM BGFX_SOURCES BX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src/amalgamated.cpp)
add_library(bgfx ${BGFX_SOURCES})
target_link_libraries(bgfx bx bimg X11)
#####################################################################


#####################################################################
# shaderc
#####################################################################
set(SHADERC_DIR ${BGFX_DIR}/tools/shaderc)
add_definitions(-DENABLE_HLSL)
include_directories(${BGFX_DIR}/3rdparty/fcpp)
include_directories(${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl)
include_directories(${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa)
include_directories(${BGFX_DIR}/3rdparty/glsl-optimizer/src/util)
include_directories(${BGFX_DIR}/3rdparty/glsl-optimizer/src)
include_directories(${BGFX_DIR}/3rdparty/glsl-optimizer/include)
include_directories(${BGFX_DIR}/3rdparty/glslang/glslang/Public)
include_directories(${BGFX_DIR}/3rdparty/glslang/glslang/Include)
include_directories(${BGFX_DIR}/3rdparty/spirv-tools)
include_directories(${BGFX_DIR}/3rdparty/spirv-tools/include)
include_directories(${BGFX_DIR}/3rdparty/spirv-headers/include)
include_directories(${BGFX_DIR}/3rdparty/spirv-tools/include/generated)
include_directories(${BGFX_DIR}/3rdparty/spirv-tools/include/spirv-tools)

set(GLSLANG ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/3rdparty/glslang)
include_directories(${BGFX_DIR}/3rdparty/glslang)
file(GLOB_RECURSE BGFX_3RDPARTY_SOURCES
	${GLSLANG}/glslang/*.cpp
	${GLSLANG}/hlsl/*.cpp
	${GLSLANG}/SPIRV/*.cpp
	${GLSLANG}/OGLCompilersDLL/*.cpp
	${BGFX_DIR}/3rdparty/spirv-cross/*.cpp
	${BGFX_DIR}/3rdparty/spirv-tools/*.cpp
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/*.cpp
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/*.c
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/mesa/*.c
	${BGFX_DIR}/3rdparty/glsl-optimizer/src/util/*.c
	${BGFX_DIR}/3rdparty/fcpp/cpp*.c)
set(NODE ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/3rdparty/glsl-optimizer/src/node)
list(REMOVE_ITEM BGFX_3RDPARTY_SOURCES
	${NODE}/compiler.cpp
	${NODE}/shader.cpp
	${NODE}/binding.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/3rdparty/glsl-optimizer/src/glsl/main.cpp
	# ${GLSLANG}/glslang/OSDependent/Unix/ossource.cpp
	${GLSLANG}/glslang/OSDependent/Windows/main.cpp
	${GLSLANG}/glslang/OSDependent/Windows/ossource.cpp
	)
file(GLOB SHADERC_SOURCES ${SHADERC_DIR}/*.cpp ${SHADERC_DIR}/*.h )
add_executable(shaderc ${SHADERC_SOURCES} ${BGFX_3RDPARTY_SOURCES})
target_link_libraries(shaderc bx bgfx pthread dl ${OPENGL_LIBRARIES})
target_compile_definitions(shaderc PUBLIC -DENABLE_OPT)
#####################################################################


#####################################################################
# imgui
#####################################################################
set(IMGUI_DIR libs/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h )
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp)
#####################################################################

#####################################################################
# nanovg
#####################################################################
set(NANOVG_DIR ${BGFX_DIR}/examples/common/nanovg)
include_directories(${NANOVG_DIR})
file(GLOB NANOVG_SOURCES ${NANOVG_DIR}/*.cpp ${NANOVG_DIR}/*.h )
add_library(nanovg ${NANOVG_SOURCES})
#####################################################################


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
file(GLOB_RECURSE SOURCES sources/*.cpp  sources/*.c sources/*.h)
if (TESTS)
	file(GLOB_RECURSE TESTS tests/*.cpp)
endif()

add_executable(Oquonie ${SOURCES} ${TESTS})
target_link_libraries(Oquonie gl3w glfw bx bgfx pthread dl  imgui pugixml fsal lz4 zlib_static vorbis ogg scribe yaml-cpp ${OPENGL_LIBRARIES} ${OPENAL_LIBRARY})

configure_file(test_rgba8888.dds test_rgba8888.dds COPYONLY)
configure_file(test_rgba8888.pvr test_rgba8888.pvr COPYONLY)
configure_file(test_rgb888.pvr test_rgb888.pvr COPYONLY)
configure_file(test_rgb565.pvr test_rgb565.pvr COPYONLY)
configure_file(test_etc1.pvr test_etc1.pvr COPYONLY)
configure_file(test_etc2.pvr test_etc2.pvr COPYONLY)
configure_file(test_pvrtc_2bpp.pvr test_pvrtc_2bpp.pvr COPYONLY)
configure_file(test_pvrtc_4bpp.pvr test_pvrtc_4bpp.pvr COPYONLY)
configure_file(test_colored.pvr test_colored.pvr COPYONLY)
configure_file(test.jpg test.jpg COPYONLY)
configure_file(test.bmp test.bmp COPYONLY)
configure_file(test.png test.png COPYONLY)

if(MSVC)
else()
	target_link_libraries(Oquonie stdc++fs)
	target_compile_options(Oquonie PRIVATE -lstdc++fs -Wno-switch)
	target_link_options(Oquonie PRIVATE -static-libstdc++)
endif()


file(GLOB files_vs "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_*.sc")
file(GLOB files_fs "${CMAKE_CURRENT_SOURCE_DIR}/shaders/fs_*.sc")

add_custom_target(
    generate-shaders
    DEPENDS ${files_fs}
    DEPENDS ${files_vs})


set(dst_dir ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders)
set(exec ${CMAKE_BINARY_DIR}/shaderc -i ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src)
file(MAKE_DIRECTORY ${dst_dir}/glsl)
file(MAKE_DIRECTORY ${dst_dir}/spv)
file(MAKE_DIRECTORY ${dst_dir}/mtl)

foreach(filename_fs ${files_fs})

get_filename_component(file ${filename_fs} NAME_WE)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  asm.js  -p 150 -o ${dst_dir}/glsl/${file}.bin --type fragment)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  linux  -p spirv -o ${dst_dir}/spv/${file}.bin --type fragment)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${BGFX_DIR}/src -f  ${filename_fs} --platform  windows --bin2c -p ps_3_0 -O 3 -o ${filename_fs}_dx9.h --type fragment)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${BGFX_DIR}/src -f  ${filename_fs} --platform  windows --bin2c -p ps_4_0 -O 3 -o ${filename_fs}_dx11.h --type fragment)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  ios  -p metal  -O 3 -o ${dst_dir}/mtl/${file}.bin --type fragment)
endforeach()
foreach(filename_vs ${files_vs})
get_filename_component(file  ${filename_vs} NAME_WE)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  asm.js -p 150 -o ${dst_dir}/glsl/${file}.bin --type vertex)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  linux -p spirv -o ${dst_dir}/spv/${file}.bin --type vertex)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src -f  ${filename_vs} --platform  windows --bin2c -p vs_3_0 -O 3 -o ${filename_vs}_dx9.h --type vertex)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src -f  ${filename_vs} --platform  windows --bin2c -p vs_4_0 -O 3 -o ${filename_vs}_dx11.h --type vertex)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  ios  -p metal  -O 3 -o ${dst_dir}/mtl/${file}.bin --type vertex)
endforeach()

#add_dependencies(FastPaint generate-shaders)
add_dependencies(generate-shaders shaderc)
