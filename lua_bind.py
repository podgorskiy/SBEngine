from pyutils import *
import os
import fnmatch

swig = ''
if is_osx():
    swig = 'swig'

elif is_linux():
    swig = 'swig'

elif is_windows():
    swig = os.path.abspath('tools/swig/win32/swig.exe')


def main():
    at_exit("Generation time:")

    dir_cpp = 'sources/Generated'

    make_clean_dir(dir_cpp)
    os.chdir(dir_cpp)

    matches = []
    for root, dirs, files in os.walk('../'):
        for filename in fnmatch.filter(files, '*.i'):
            matches.append(os.path.join(root, filename))

    for f in matches:
        print(f)
        generate(f)


def generate(path):
    includes = [
        "libs/bgfx/examples/common",
        "libs/bgfx/examples/3rdparty",
        "libs/bgfx/include",
        "libs/bimg/include",
        "libs/glfw/include",
        "libs/gl3w/include",
        "libs/glm",
        "libs/imgui",
        "libs/lz4",
        "libs/SimpleText/include",
        "libs/scriberlib/include",
        "libs/scriberlib/3rdparty/utf8",
        "libs/yaml-cpp/include",
        "libs/spdlog/include",
        "libs/fsal/sources",
        "libs/zlib",
        "sources",
    ]

    try:
        inc = []
        for i in includes:
            inc.append("-I../../"+i)
        misc.execute([swig, '-lua', '-c++', '-cppext', 'cpp'] + ['-o', 'lua_wrap.cpp'] + ['-oh', 'lua_wrap.h'] +  inc + ['-outcurrentdir', path])
    except ValueError:
        print("\n\nFailed to generate wrapper for: {0}".format(path))


if __name__ == "__main__":
    main()
