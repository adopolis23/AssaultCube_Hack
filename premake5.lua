workspace "AssaultCube_Hack"
    architecture "x86"
    configurations { "Debug", "Release" }


project "AssaultCube_Hack_DLL"
    kind "SharedLib"
    language "C++"
    cppdialect "C++23"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    files {
        "AssaultCube_Hack_DLL/src/**.h",
        "AssaultCube_Hack_DLL/src/**.cpp",
        "AssaultCube_Hack_DLL/**.cpp",
        "*.lua"
    }

    includedirs {
        "AssaultCube_Hack_DLL/src",
        "AssaultCube_Hack_DLL/src/proc",
        "AssaultCube_Hack_DLL/src/mem"
    }

    removefiles { "DLLInjector**" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"


project "DLL_Injector"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    files {
        "DLLInjector/src/**.h",
        "DLLInjector/src/**.cpp",
    }

    includedirs {
        "DLLInjector/src",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"