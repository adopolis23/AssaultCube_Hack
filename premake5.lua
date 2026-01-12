workspace "AssaultCube_Hack"
    architecture "x86"
    configurations { "Debug", "Release" }


project "AssaultCube_Hack"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    files {
        "src/**.h",
        "src/**.cpp",
        "*.lua"
    }

    includedirs {
        "src",
        "src/proc"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"