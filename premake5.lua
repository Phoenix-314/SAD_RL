include "dependencies/conandeps.premake5.lua"

workspace "SAD_CPP"
    configurations { "Debug", "Release" }
    architecture "x64"

    project "SAD_CPP"
        kind "ConsoleApp"
        --profile "On"
        language "C++"
        cppdialect "C++20"

        targetdir   "build/%{cfg.buildcfg}/bin"
        objdir      "build/%{cfg.buildcfg}/obj"

        location "project"
        files { "src/**.cpp", "include/**.h" }
        includedirs { "include" }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"
        filter {}

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
        filter {}

        conan_setup()
        --linkoptions { "/IGNORE:4099" }