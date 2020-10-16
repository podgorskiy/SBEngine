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

set(CMAKE_CXX_STANDARD 17)

# list(APPEND CMAKE_PREFIX_PATH libs/backward-cpp/)

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)


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
	# -fno-rtti
	set(RELEASE_OPTIONS -g -Ofast -fPIC -funsafe-math-optimizations -fno-strict-aliasing  -ffast-math -msse2 -msse3 -msse4)
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
# Externa libs
##############################################################
add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)
add_subdirectory (libs/fsal)
add_subdirectory (libs/scriberlib)
add_subdirectory (libs/yaml-cpp)
add_subdirectory (libs/spdlog)

target_include_directories(fsal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/lz4/lib)


#####################################################################
# bx
#####################################################################
set(BX_DIR libs/bx)
set(BX_3DPARTY ${BX_DIR}/3rdparty)
set(BX_INCLUDE ${BX_DIR}/include)
file(GLOB BX_SOURCES ${BX_DIR}/src/*.cpp ${BX_INCLUDE}/*.h)
list(REMOVE_ITEM BX_SOURCES BX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${BX_DIR}/src/amalgamated.cpp)
add_library(bx ${BX_SOURCES})
target_include_directories(bx PUBLIC ${BX_INCLUDE})
target_include_directories(bx PRIVATE ${BX_3DPARTY})
if(MSVC)
target_include_directories(bx PUBLIC ${BX_INCLUDE}/compat/msvc)
endif()
#####################################################################


#####################################################################
# shaderc
#####################################################################
set(BGFX_DIR libs/bgfx)
set(BIMG_DIR libs/bimg)
set(BIMG_INCLUDE ${BIMG_DIR}/include)
set(BGFX_INCLUDE ${BGFX_DIR}/include)
set(SHADERC_DIR ${BGFX_DIR}/tools/shaderc)

file(GLOB_RECURSE BGFX_SPIRV_TOOLS ${BGFX_DIR}/3rdparty/spirv-tools/*.*)
add_library(spirv-opt ${BGFX_SPIRV_TOOLS})
target_include_directories(spirv-opt PUBLIC  ${BGFX_DIR}/3rdparty/spirv-tools/include)
target_include_directories(spirv-opt PRIVATE ${BGFX_DIR}/3rdparty/spirv-tools/include/generated)
target_include_directories(spirv-opt PRIVATE ${BGFX_DIR}/3rdparty/spirv-headers/include)
target_include_directories(spirv-opt PRIVATE ${BGFX_DIR}/3rdparty/spirv-tools/source)
target_include_directories(spirv-opt PRIVATE ${BGFX_DIR}/3rdparty/spirv-tools)
target_compile_options(spirv-opt PRIVATE -Wno-switch)

file(GLOB_RECURSE BGFX_SPIRV_CROSS ${BGFX_DIR}/3rdparty/spirv-cross/*.*)
add_library(spirv-cross ${BGFX_SPIRV_CROSS})
target_include_directories(spirv-cross PUBLIC ${BGFX_DIR}/3rdparty/spirv-cross/include)

set(GLSLANG ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/3rdparty/glslang)
file(GLOB_RECURSE BGFX_GLSLANG ${GLSLANG}/glslang/*.* ${GLSLANG}/hlsl/*.* ${GLSLANG}/SPIRV/*.* ${GLSLANG}/OGLCompilersDLL/*.*)
list(REMOVE_ITEM BGFX_GLSLANG
	# ${GLSLANG}/glslang/OSDependent/Unix/ossource.cpp
	${GLSLANG}/glslang/OSDependent/Unix/main.cpp
	${GLSLANG}/glslang/OSDependent/Windows/main.cpp
	${GLSLANG}/glslang/OSDependent/Windows/ossource.cpp
		)
add_library(glslang ${BGFX_GLSLANG})
target_compile_definitions(glslang PRIVATE ENABLE_HLSL ENABLE_OPT)
target_include_directories(glslang PUBLIC ${GLSLANG})
target_include_directories(glslang PRIVATE ${BGFX_DIR}/3rdparty/spirv-tools/include)
target_include_directories(glslang PRIVATE ${BGFX_DIR}/3rdparty/spirv-tools/source)

set(GLSL_OPIMIZER ${BGFX_DIR}/3rdparty/glsl-optimizer)
file(GLOB_RECURSE BGFX_GLSL_OPTIMIZER
		${GLSL_OPIMIZER}/src/*.*
		${GLSL_OPIMIZER}/src/glsl/*.*
		${GLSL_OPIMIZER}/src/mesa/*.*
		${GLSL_OPIMIZER}/src/util/*.*
		)
set(NODE ${CMAKE_CURRENT_SOURCE_DIR}/${GLSL_OPIMIZER}/src/node)
list(REMOVE_ITEM BGFX_GLSL_OPTIMIZER
	${NODE}/compiler.cpp
	${NODE}/shader.cpp
	${NODE}/binding.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/${GLSL_OPIMIZER}/src/glsl/main.cpp
	)
add_library(glsl-optimizer ${BGFX_GLSL_OPTIMIZER})
target_include_directories(glsl-optimizer PRIVATE
		${GLSL_OPIMIZER}/src
		${GLSL_OPIMIZER}/src/mesa
		${GLSL_OPIMIZER}/src/mapi
		)
target_include_directories(glsl-optimizer PUBLIC ${GLSL_OPIMIZER}/include ${GLSL_OPIMIZER}/src/glsl)
target_compile_options(glsl-optimizer PRIVATE -fno-strict-aliasing)

set(FCPP ${BGFX_DIR}/3rdparty/fcpp)
add_library(fcpp ${FCPP}/cpp1.c ${FCPP}/cpp2.c ${FCPP}/cpp3.c ${FCPP}/cpp4.c ${FCPP}/cpp5.c ${FCPP}/cpp6.c)
target_compile_definitions(fcpp PRIVATE NINCLUDE=64 NWORK=65536 NBUFF=65536 OLD_PREPROCESSOR=0)

file(GLOB BGFX_ADDITIONAL
	${BGFX_DIR}/src/vertexlayout.*
	${BGFX_DIR}/src/shader_spirv.*)
file(GLOB SHADERC_SOURCES ${SHADERC_DIR}/*.cpp ${SHADERC_DIR}/*.h ${BGFX_ADDITIONAL})
add_executable(shaderc ${SHADERC_SOURCES} ${BGFX_3RDPARTY_SOURCES})
target_include_directories(shaderc PRIVATE ${GLSLANG}/glslang/Public)
target_include_directories(shaderc PRIVATE ${GLSLANG}/glslang/Include)
target_include_directories(shaderc PRIVATE ${BGFX_DIR}/3rdparty/dxsdk/include)
target_include_directories(shaderc PRIVATE ${BGFX_DIR}/include)
target_include_directories(shaderc PRIVATE ${BGFX_DIR}/src)
target_include_directories(shaderc PRIVATE ${BIMG_INCLUDE})
target_include_directories(shaderc PRIVATE ${BGFX_DIR}/3rdparty/spirv-cross)

target_link_libraries(shaderc PRIVATE bx glsl-optimizer spirv-cross spirv-opt glslang fcpp pthread dl)

target_include_directories(shaderc PRIVATE ${BGFX_DIR}/3rdparty/fcpp)
#####################################################################



#####################################################################
# lua
#####################################################################
set(LUA_ROOT libs/lua)
file(GLOB LUA_SRCS ${LUA_ROOT}/*.c ${LUA_ROOT}/*.h)
add_library(lua STATIC ${LUA_SRCS})
target_include_directories(lua PUBLIC ${LUA_ROOT})


#####################################################################
# bimg
#####################################################################
set(BIMG_DIR libs/bimg)
set(BIMG_INCLUDE ${BIMG_DIR}/include)
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
target_link_libraries(bimg PUBLIC bx)
target_include_directories(bimg PUBLIC ${BIMG_INCLUDE})
target_include_directories(bimg PRIVATE ${BGFX_DIR}/3rdparty)
target_include_directories(bimg PRIVATE ${BIMG_DIR}/3rdparty)
target_include_directories(bimg PRIVATE ${BIMG_DIR}/3rdparty/iqa/include)
target_include_directories(bimg PRIVATE ${BIMG_DIR}/3rdparty/nvtt)
target_include_directories(bimg PRIVATE ${BIMG_DIR}/3rdparty/astc)
target_include_directories(bimg PRIVATE ${BIMG_DIR}/3rdparty/astc-codec)
target_include_directories(bimg PRIVATE ${BIMG_DIR}/3rdparty/astc-codec/include)
#####################################################################

#####################################################################
# bgfx
#####################################################################
set(BGFX_DIR libs/bgfx)
set(BGFX_INCLUDE ${BGFX_DIR}/include)
file(GLOB BGFX_SOURCES
	${BGFX_DIR}/src/*.cpp
	${BGFX_DIR}/src/*.h
	${BGFX_INCLUDE}/*.h
	${BGFX_DIR}/src/*.inl
	)
list(REMOVE_ITEM BGFX_SOURCES BX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src/amalgamated.cpp)
add_library(bgfx ${BGFX_SOURCES})
target_link_libraries(bgfx PRIVATE X11)
target_link_libraries(bgfx PUBLIC bx bimg)
target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_DEBUG=1)
target_include_directories(bgfx PRIVATE ${BGFX_DIR}/tools/shaderc)
target_include_directories(bgfx PRIVATE ${BGFX_DIR}/3rdparty)
target_include_directories(bgfx PRIVATE ${BGFX_DIR}/3rdparty/khronos)
target_include_directories(bgfx PRIVATE ${BGFX_DIR}/3rdparty/spirv-cross)
target_include_directories(bgfx PRIVATE ${BGFX_DIR}/3rdparty/dxsdk/include)
target_include_directories(bgfx PUBLIC libs/imgui)
target_include_directories(bgfx PUBLIC ${BGFX_INCLUDE})
if(MSVC)
target_compile_definitions(bgfx PUBLIC ${BGFX_INCLUDE}/compat/msvc)
endif()
#####################################################################


#####################################################################
# imgui
#####################################################################
set(IMGUI_DIR ${BGFX_DIR}/3rdparty/dear-imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h ${BGFX_DIR}/examples/common/imgui/*.h ${BGFX_DIR}/examples/common/imgui/*.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${BGFX_DIR}/examples/common)
target_include_directories(imgui PUBLIC ${BGFX_DIR}/3rdparty)
target_link_libraries(imgui PRIVATE bgfx bimg bx)
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
target_link_libraries(Oquonie PRIVATE gl3w glfw bx bgfx pthread dl  imgui pugixml fsal lz4 zlib_static vorbis ogg scribe yaml-cpp lua ${OPENGL_LIBRARIES} ${OPENAL_LIBRARY})
target_link_libraries(Oquonie PRIVATE spdlog::spdlog)


##############################################################
# Includes
##############################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/gl3w/include)
include_directories(${CMAKE_BINARY_DIR}/libs/gl3w/include)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/fsal/sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/pugixml/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/lz4/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/minimp3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libogg/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/entt/single_include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libvorbis/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources/Game)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/doctest/doctest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/scriberlib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/scriberlib/3rdparty/utf8)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/yaml-cpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/LuaBridge/Source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/backward-cpp)



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
configure_file(test.png test.png COPYONLY)

if(MSVC)
else()
	target_link_libraries(Oquonie PRIVATE stdc++fs)
	target_compile_options(Oquonie PRIVATE -lstdc++fs -Wno-switch)
	target_link_options(Oquonie PRIVATE -static-libstdc++)
	target_link_libraries(Oquonie PRIVATE dw)
endif()


file(GLOB_RECURSE files_vs "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_*.sc")
file(GLOB_RECURSE files_fs "${CMAKE_CURRENT_SOURCE_DIR}/shaders/fs_*.sc")

add_custom_target(
    generate-shaders
    DEPENDS ${files_fs}
    DEPENDS ${files_vs})


set(dst_dir ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders)
set(exec ${CMAKE_BINARY_DIR}/shaderc -i ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src -i ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
file(MAKE_DIRECTORY ${dst_dir}/glsl)
file(MAKE_DIRECTORY ${dst_dir}/essl)
file(MAKE_DIRECTORY ${dst_dir}/spirv)
file(MAKE_DIRECTORY ${dst_dir}/metal)

foreach(filename_fs ${files_fs})

get_filename_component(file ${filename_fs} NAME_WE)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  asm.js  -p 150 -o ${dst_dir}/essl/${file}.bin --type fragment)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  linux  -p 150 -o ${dst_dir}/glsl/${file}.bin --type fragment)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  linux  -p spirv -o ${dst_dir}/spirv/${file}.bin --type fragment)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${BGFX_DIR}/src -f  ${filename_fs} --platform  windows --bin2c -p ps_3_0 -O 3 -o ${filename_fs}_dx9.h --type fragment)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${BGFX_DIR}/src -f  ${filename_fs} --platform  windows --bin2c -p ps_4_0 -O 3 -o ${filename_fs}_dx11.h --type fragment)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec}  -f  ${filename_fs} --platform  ios  -p metal  -O 3 -o ${dst_dir}/metal/${file}.bin --type fragment)
endforeach()
foreach(filename_vs ${files_vs})
get_filename_component(file  ${filename_vs} NAME_WE)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  asm.js -p 150 -o ${dst_dir}/essl/${file}.bin --type vertex)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  linux -p 150 -o ${dst_dir}/glsl/${file}.bin --type vertex)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  linux -p spirv -o ${dst_dir}/spirv/${file}.bin --type vertex)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src -f  ${filename_vs} --platform  windows --bin2c -p vs_3_0 -O 3 -o ${filename_vs}_dx9.h --type vertex)
#add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -i ${CMAKE_CURRENT_SOURCE_DIR}/${BGFX_DIR}/src -f  ${filename_vs} --platform  windows --bin2c -p vs_4_0 -O 3 -o ${filename_vs}_dx11.h --type vertex)
add_custom_command(TARGET generate-shaders PRE_BUILD COMMAND ${exec} -f  ${filename_vs} --platform  ios  -p metal  -O 3 -o ${dst_dir}/metal/${file}.bin --type vertex)
endforeach()

# add_dependencies(FastPaint generate-shaders)
# add_dependencies(generate-shaders shaderc)
