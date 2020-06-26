local PROJ_DIR     = path.getabsolute(".")
local LIB_DIR      = path.join(PROJ_DIR, "libs")

BGFX_DIR     = path.join(LIB_DIR, "bgfx")
BX_DIR       = path.join(LIB_DIR, "bx")
BIMG_DIR     = path.join(LIB_DIR, "bimg")

local BGFX_SCRIPTS_DIR = path.join(BGFX_DIR, "scripts/")
local BX_SCRIPTS_DIR   = path.join(BX_DIR, "scripts/")
local BIMG_SCRIPTS_DIR = path.join(BIMG_DIR, "scripts/")

local BGFX_BUILD_DIR = path.join(BGFX_DIR, ".build")
local BGFX_THIRD_PARTY_DIR = path.join(BGFX_DIR, "3rdparty")


function copyLib()
end


solution "Oquonie"
	basedir "."

	targetdir "build"

	objdir "build/obj"

	configurations{
		"Debug",
		"Release",
		"Retail",
	}

    configuration { "Debug" }
        targetdir "build/Debug"

    configuration { "Release" }
        targetdir "build/Release"

    configuration { "Retail" }
        targetdir "build/Retail"

    configuration { "Release or Retail" }
		flags { "Optimize", "OptimizeSpeed", "No64BitChecks" }

	configuration{}

	if _ACTION == "xcode*" then
		platforms {
			"Universal",
		}
	else
		platforms {
			"x64",
			"Native",
		}
	end

	language "C++"

	flags{
		"FullSymbols",
		"StaticRuntime",
		"Symbols",
		"FloatFast",
		"NoRTTI",
		"NoWinMD",
		"NoWinRT",
		"NoEditAndContinue"
	}

	debugdir (path.join(PROJ_DIR, "build"))

	defines
	{
		-- "ENTRY_CONFIG_IMPLEMENT_MAIN=1",
		"ENTRY_DEFAULT_WIDTH=1920",
		"ENTRY_DEFAULT_HEIGHT=1027",
		"BGFX_CONFIG_RENDERER_OPENGL=21",
	}

	startproject "Oquonie"

    dofile (path.join(BX_DIR, "scripts/toolchain.lua"))
    if not toolchain("build", BGFX_THIRD_PARTY_DIR) then
        return -- no action specified
    end

group "libs"
dofile (path.join(BGFX_SCRIPTS_DIR, "bgfx.lua"))
dofile (path.join(BX_SCRIPTS_DIR, "bx.lua"))
dofile (path.join(BIMG_SCRIPTS_DIR, "bimg.lua"))
dofile (path.join(BIMG_SCRIPTS_DIR, "bimg_decode.lua"))
dofile (path.join(BGFX_SCRIPTS_DIR, "example-common.lua"))

bgfxProject("", "StaticLib", {})


project "Oquonie"
	kind "ConsoleApp"
	language "C++"
	files {
		path.join(PROJ_DIR, "sources/*.h"),
		path.join(PROJ_DIR, "sources/*.cpp")
	}

	includedirs {
		path.join(PROJ_DIR, "sources"),
		path.join(PROJ_DIR, "sources/Game"),
		path.join(BGFX_DIR, "3rdparty"),
		path.join(BGFX_DIR, "include"),
		path.join(BX_DIR, "include"),
		path.join(BIMG_DIR, "include"),
	}

	links { "bgfx", "bx", "bimg", "bimg_decode"}

	configuration { "vs20*", "x32 or x64" }
		links {
			"gdi32",
			"psapi",
		}

	configuration { "linux-* or freebsd or cmake" }
		links {
			"X11",
			"GL",
			"pthread",
		}

	configuration { "asmjs" }
		kind "ConsoleApp"
		targetextension ".bc"

	configuration {"Debug"}
		debugdir "."
		debugargs { "" }
		defines { "DEBUG" }
		flags { "Symbols"}

	configuration {"Release"}
		defines { "NDEBUG" }
		flags { "Optimize" }

function exampleProjectDefaults()

	debugdir (path.join(BGFX_DIR, "examples/runtime"))

	includedirs {
		path.join(BX_DIR,   "include"),
		path.join(BIMG_DIR, "include"),
		path.join(BGFX_DIR, "include"),
		path.join(BGFX_DIR, "3rdparty"),
		path.join(BGFX_DIR, "examples/common"),
	}

	flags {
		"FatalWarnings",
	}

	links {
		"example-glue",
		"example-common",
		"bgfx",
		"bimg_decode",
		"bimg",
		"bx",
	}

	if _OPTIONS["with-sdl"] then
		defines { "ENTRY_CONFIG_USE_SDL=1" }
		links   { "SDL2" }

		configuration { "linux or freebsd" }
			if _OPTIONS["with-wayland"]  then
				links {
					"wayland-egl",
				}
			end

		configuration { "osx" }
			libdirs { "$(SDL2_DIR)/lib" }

		configuration {}
	end

	if _OPTIONS["with-glfw"] then
		defines { "ENTRY_CONFIG_USE_GLFW=1" }
		links   { "glfw3" }

		configuration { "linux or freebsd" }
			if _OPTIONS["with-wayland"] then
				links {
					"wayland-egl",
				}
			else
				links {
					"Xrandr",
					"Xinerama",
					"Xi",
					"Xxf86vm",
					"Xcursor",
				}
			end

		configuration { "osx" }
			linkoptions {
				"-framework CoreVideo",
				"-framework IOKit",
			}

		configuration {}
	end

	configuration { "vs*", "x32 or x64" }
		linkoptions {
			"/ignore:4199", -- LNK4199: /DELAYLOAD:*.dll ignored; no imports found from *.dll
		}
		links { -- this is needed only for testing with GLES2/3 on Windows with VS2008
			"DelayImp",
		}

	configuration { "vs201*", "x32 or x64" }
		linkoptions { -- this is needed only for testing with GLES2/3 on Windows with VS201x
			"/DELAYLOAD:\"libEGL.dll\"",
			"/DELAYLOAD:\"libGLESv2.dll\"",
		}

	configuration { "mingw*" }
		targetextension ".exe"
		links {
			"gdi32",
			"psapi",
		}

	configuration { "vs20*", "x32 or x64" }
		links {
			"gdi32",
			"psapi",
		}

	configuration { "durango" }
		links {
			"d3d11_x",
			"d3d12_x",
			"combase",
			"kernelx",
		}

	configuration { "winstore*" }
		removelinks {
			"DelayImp",
			"gdi32",
			"psapi"
		}
		links {
			"d3d11",
			"d3d12",
			"dxgi"
		}
		linkoptions {
			"/ignore:4264" -- LNK4264: archiving object file compiled with /ZW into a static library; note that when authoring Windows Runtime types it is not recommended to link with a static library that contains Windows Runtime metadata
		}

	-- WinRT targets need their own output directories or build files stomp over each other
	configuration { "x32", "winstore*" }
		targetdir (path.join(BGFX_BUILD_DIR, "win32_" .. _ACTION, "bin", _name))
		objdir (path.join(BGFX_BUILD_DIR, "win32_" .. _ACTION, "obj", _name))

	configuration { "x64", "winstore*" }
		targetdir (path.join(BGFX_BUILD_DIR, "win64_" .. _ACTION, "bin", _name))
		objdir (path.join(BGFX_BUILD_DIR, "win64_" .. _ACTION, "obj", _name))

	configuration { "ARM", "winstore*" }
		targetdir (path.join(BGFX_BUILD_DIR, "arm_" .. _ACTION, "bin", _name))
		objdir (path.join(BGFX_BUILD_DIR, "arm_" .. _ACTION, "obj", _name))

	configuration { "mingw-clang" }
		kind "ConsoleApp"

	configuration { "android*" }
		kind "ConsoleApp"
		targetextension ".so"
		linkoptions {
			"-shared",
		}
		links {
			"EGL",
			"GLESv2",
		}

	configuration { "asmjs" }
		kind "ConsoleApp"

		linkoptions {
			"-s TOTAL_MEMORY=256MB",
			"--memory-init-file 1",
		}

		removeflags {
			"OptimizeSpeed",
		}

		flags {
			"Optimize"
		}

	configuration { "linux-* or freebsd" }
		printf("links X11")
		links {
			"X11",
			"GL",
			"pthread",
		}

	configuration { "rpi" }
		links {
			"X11",
			"brcmGLESv2",
			"brcmEGL",
			"bcm_host",
			"vcos",
			"vchiq_arm",
			"pthread",
		}

	configuration { "osx" }
		linkoptions {
			"-framework Cocoa",
			"-framework QuartzCore",
			"-framework OpenGL",
			"-weak_framework Metal",
		}

	configuration { "ios* or tvos*" }
		kind "ConsoleApp"
		linkoptions {
			"-framework CoreFoundation",
			"-framework Foundation",
			"-framework OpenGLES",
			"-framework UIKit",
			"-framework QuartzCore",
			"-weak_framework Metal",
		}

	configuration { "xcode*", "ios" }
		kind "WindowedApp"
		files {
			path.join(BGFX_DIR, "examples/runtime/iOS-Info.plist"),
		}

	configuration { "xcode*", "tvos" }
		kind "WindowedApp"
		files {
			path.join(BGFX_DIR, "examples/runtime/tvOS-Info.plist"),
		}


	configuration { "qnx*" }
		targetextension ""
		links {
			"EGL",
			"GLESv2",
		}

	configuration {}

	strip()
end

function exampleProject(_combined, ...)

	if _combined then

		project ("examples")
			uuid (os.uuid("examples"))
			kind "WindowedApp"

		for _, name in ipairs({...}) do

			files {
				path.join(BGFX_DIR, "examples", name, "**.c"),
				path.join(BGFX_DIR, "examples", name, "**.cpp"),
				path.join(BGFX_DIR, "examples", name, "**.h"),
			}

			removefiles {
				path.join(BGFX_DIR, "examples", name, "**.bin.h"),
			}

		end

		files {
			path.join(BGFX_DIR, "examples/25-c99/helloworld.c"), -- hack for _main_
		}

		exampleProjectDefaults()

	else

		for _, name in ipairs({...}) do
			project ("example-" .. name)
				uuid (os.uuid("example-" .. name))
				kind "WindowedApp"

			files {
				path.join(BGFX_DIR, "examples", name, "**.c"),
				path.join(BGFX_DIR, "examples", name, "**.cpp"),
				path.join(BGFX_DIR, "examples", name, "**.h"),
			}

			removefiles {
				path.join(BGFX_DIR, "examples", name, "**.bin.h"),
			}

			defines {
				"ENTRY_CONFIG_IMPLEMENT_MAIN=1",
			}

			exampleProjectDefaults()
		end
	end

end


	exampleProject(true
		, "00-helloworld"
		, "01-cubes"
		, "02-metaballs"
		, "03-raymarch"
		, "04-mesh"
		, "05-instancing"
		, "06-bump"
		, "07-callback"
		, "08-update"
		, "09-hdr"
		, "10-font"
		, "11-fontsdf"
		, "12-lod"
		, "13-stencil"
		, "14-shadowvolumes"
		, "15-shadowmaps-simple"
		, "16-shadowmaps"
		, "17-drawstress"
		, "18-ibl"
		, "19-oit"
		, "20-nanovg"
		, "21-deferred"
		, "22-windows"
		, "23-vectordisplay"
		, "24-nbody"
		, "26-occlusion"
		, "27-terrain"
		, "28-wireframe"
		, "29-debugdraw"
		, "30-picking"
		, "31-rsm"
		, "32-particles"
		, "33-pom"
		, "34-mvs"
		, "35-dynamic"
		, "36-sky"
		, "37-gpudrivenrendering"
		, "38-bloom"
		, "39-assao"
		, "40-svt"
		, "41-tess"
		)
