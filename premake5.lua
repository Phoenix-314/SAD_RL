newoption {
    trigger = "browser",
    description = "Generate an Emscripten browser build"
}

local dependency_dir = _OPTIONS["browser"] and "dependencies/emscripten" or "dependencies"
include(dependency_dir .. "/conandeps.premake5.lua")

workspace "SAD_CPP"
    configurations { "Debug", "Release" }

    filter "action:vs*"
        architecture "x64"
    filter {}

    project "SAD_CPP"
        kind "ConsoleApp"
        profile "On" --
        language "C++"
        cppdialect "C++20"

        targetdir   "build/%{cfg.buildcfg}/bin"
        objdir      "build/%{cfg.buildcfg}/obj"

        location "project"
        files { "src/**.cpp", "include/**.h" }
        includedirs { "include" }

        filter "options:browser"
            profile "Off"
            targetextension ".html"
            targetdir "docs" -- "build/%{cfg.buildcfg}/browser" -- built in docs folder for github pages access
            toolset "clang"
            buildoptions { "-pthread" }
            linkoptions {
                "-pthread",
                "-sALLOW_MEMORY_GROWTH=1",
                "-sSTACK_SIZE=1048576",
                "-sNO_EXIT_RUNTIME=1",
                "--preload-file", "../assets@/assets",
            }
        filter {}

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"
        filter {}

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
        filter {}

        if _OPTIONS["browser"] then
            filter "configurations:Debug"
                conan_setup("debug_wasm32")
            filter {}
            filter "configurations:Release"
                conan_setup("release_wasm32")
            filter {}
        else
            conan_setup()
        end
        if not _OPTIONS["browser"] then
            linkoptions { "/IGNORE:4099" }
        end