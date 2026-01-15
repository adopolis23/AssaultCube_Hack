workspace "AssaultCube_Hack"
   architecture "x86"
   configurations { "Debug", "Release" }
   startproject "DLLInjector"

-- DLL Injector project
project "DLLInjector"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   targetdir "bin/%{cfg.buildcfg}"
   objdir "bin-int/%{cfg.buildcfg}"


   -- Include files from DLLInjector folder
   files { "DLLInjector/**.cpp", "DLLInjector/**.h" }

   -- Include directories if needed
   includedirs { "DLLInjector" }

-- AssaultCube Hack DLL project
project "AssaultCube_Hack_DLL"
   kind "SharedLib"   -- DLL
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   targetdir "bin/%{cfg.buildcfg}"
   objdir "bin-int/%{cfg.buildcfg}"

   -- Include files from AssaultCube_Hack_DLL folder
   files { "AssaultCube_Hack_DLL/**.cpp", "AssaultCube_Hack_DLL/**.h" }

   -- Include directories if needed
   includedirs { "AssaultCube_Hack_DLL", "AssaultCube_Hack_DLL/src/mem", "AssaultCube_Hack_DLL/src/proc", "AssaultCube_Hack_DLL/src/include" }
