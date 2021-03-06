workspace "OpenGLScene"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Game"
    location "Game"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "Vendor/stb/include",
        "Vendor/nlohmann/include",
        "Vendor/sdl2/include",
        "Vendor/glew/include",
        "Vendor/spdlog/include",
        "Vendor/glm",
        "Vendor/assimp/include"
    }

    libdirs
    {
        "Vendor/sdl2/lib/x64",
        "Vendor/glew/lib/Release/x64",
        "Vendor/assimp/lib/RelWithDebInfo"
    }

    links 
    {
        "glew32s",
        "SDL2main",
        "SDL2", 
        "opengl32",
        "zlibstatic",
        "IrrXML",
        "assimp-vc142-mt"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        nuget { "Microsoft.glTF.CPP:1.6.3.1", "rapidjson.temprelease:0.0.2.20" }

        defines 
        {
            "_CONSOLE"
        }

    filter "configurations:Debug"
        defines "_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "_RELEASE"
        symbols "On"