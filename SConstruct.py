import fnmatch
import os
import time
import atexit
import sys
import scons
sys.path.insert(0, scons.__path__._path[0])
from scons.SCons.Environment import Environment
from scons.SCons import Script


release = False


if release:
    optimization = ['-O2', '-DNDEBUG']
    debug = '-g0'
    lto = "1"
    closure = "0"
    assertions = "0"
    demangle = "0"
else:
    optimization = ['-O0']
    debug = '-g3'
    lto = "0"
    closure = "0"
    assertions = "2"
    demangle = "1"


def main():
    env = Environment(ENV=os.environ, tools=['gcc', 'g++', 'gnulink', 'ar', 'gas'])

    env.Replace(CC="emcc")
    env.Replace(CXX="em++")
    env.Replace(LINK="emcc")

    env.Replace(AR="emar")
    env.Replace(RANLIB="emranlib")

    env.Replace(LIBLINKPREFIX="")
    env.Replace(LIBPREFIX="")
    env.Replace(LIBLINKSUFFIX=".bc")
    env.Replace(LIBSUFFIX=".bc")
    env.Replace(OBJSUFFIX=".o")
    env.Replace(PROGSUFFIX=".html")

    env.Append(CPPFLAGS=optimization + ['-DIMGUI_IMPL_OPENGL_ES3'])
    env.Append(LINKFLAGS=[
        optimization,
        debug,
        "-lGL",
        "-lglfw",
        "-lGLEW",
        '-s', 'USE_GLFW=3',
        "-s", "ASSERTIONS=" + assertions,
        "-s", "DEMANGLE_SUPPORT=" + demangle,
        "-s", "TOTAL_MEMORY=%d" % (64 * 1024 * (1024 // 64) * 300),
        "-s", "EXTRA_EXPORTED_RUNTIME_METHODS=[\"ccall\", \"cwrap\"]",
        "--llvm-lto", lto,
        "--closure", closure,
        "-s", "NO_EXIT_RUNTIME=1",
        "-s", "FULL_ES3=1",
        "-s", "DISABLE_EXCEPTION_CATCHING=1",
        "-s", "EXPORTED_FUNCTIONS=\"['_main']\"",
        "-s", "MAX_WEBGL_VERSION=2",
        "-s", "MIN_WEBGL_VERSION=2",
        "-s", "USE_WEBGL2=1",
        "--preload-file", "resources"]
    )

    timeStart = time.time()
    atexit.register(print_information_on_build_is_finished, timeStart)

    Includes = [
        "libs/glm",
        "libs/imgui",
        "libs/SimpleText/include",
        "libs/stb",
        "libs/fsal/sources",
        "libs/spdlog/include",
        "libs/pugixml/src",
        "libs/lz4/lib",
        "libs/zlib",
        "libs/minimp3",
        "libs/libogg/include",
        "libs/libvorbis/include",
        "libs/doctest/doctest",
        "libs/scriberlib/include",
        "libs/yaml-cpp/include",
        "sources",
        "sources/Game",
    ]

    imguiSources = [
        "libs/imgui/imgui.cpp",
        "libs/imgui/imgui_demo.cpp",
        "libs/imgui/imgui_widgets.cpp",
        "libs/imgui/imgui_draw.cpp",
        "libs/imgui/examples/imgui_impl_opengl3.cpp",
        "libs/imgui/examples/imgui_impl_glfw.cpp",
    ]

    lz4Sources = [
        "libs/lz4/lib/lz4.c",
    ]

    sourcesPath = "sources"
    files = glob_r(sourcesPath, "*.cpp")

    env.Library('imgui', imguiSources, CPPPATH=Includes)
    env.Library('lz4', lz4Sources)
    env.Library('stb', "sources/stb_compile_unit.c", CPPPATH=["libs/stb"])

    env.Library('freetype',
                glob_r("libs/scriberlib/3rdparty/freetype", "*.c"),
                CPPPATH=["libs/scriberlib/3rdparty/freetype"],
                CPPFLAGS=["-DHAVE_OT", "-DHB_NO_MT", "-DHAVE_FREETYPE", "-DHAVE_UCDN", '-Wno-undefined-bool-conversion', '-Wno-null-dereference'])

    env.Library('scriberlib',
                glob_r("libs/scriberlib/sources", "*.cpp") +
                glob_r("libs/scriberlib/3rdparty/harfbuzz", "*.c*") +
                glob_r("libs/scriberlib/3rdparty/harfbuzz/hb-ucdn", "*.c*"),
                CPPPATH=[
                    "libs/scriberlib/include",
                    "libs/scriberlib/3rdparty/freetype",
                    "libs/scriberlib/3rdparty/harfbuzz",
                    "libs/scriberlib/3rdparty/harfbuzz/hb-ucdn",
                    "libs/scriberlib/3rdparty/minibidi",
                    "libs/scriberlib/3rdparty/utf8",
                    "libs/scriberlib/3rdparty/xxhash",
                ],
                CPPFLAGS=['-std=c++14', "-DHAVE_OT", "-DHB_NO_MT", "-DHAVE_FREETYPE", "-DHAVE_UCDN", '-Wno-undefined-bool-conversion', '-Wno-null-dereference'])

    env.Library('fsal',
                glob_r("libs/fsal/sources", "*.cpp"),
                CPPPATH=[
                    "libs/fsal/sources",
                    "libs/lz4/lib",
                    "libs/zlib",
                ],
                CPPFLAGS=['-std=c++14'])

    program = env.Program('oquonie', files, LIBS=['imgui', 'lz4', 'scriberlib', 'freetype', 'fsal', 'stb'], CPPFLAGS=optimization + ['-std=c++14',  debug], LIBPATH='.', CPPPATH = Includes)


def print_information_on_build_is_finished(start_time):
    failures = Script.GetBuildFailures()
    for failure in failures:
        print("Target [%s] failed: %s" % (failure.node, failure.errstr))
    time_delta = time.gmtime(time.time() - start_time)
    print(time.strftime("Build time: %M minutes %S seconds", time_delta))


def glob_r(path, filter):
    matches = []
    for root, dirnames, filenames in os.walk(path):
        for filename in fnmatch.filter(filenames, filter):
            matches.append(os.path.join(root, filename))
    return matches


main()
