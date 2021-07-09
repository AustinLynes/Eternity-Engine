-- SOLUTION
workspace "Eternity Engine"
    architecture "x64"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

-- PROJECTS
-- TUTORIAL 1 -- CREATE A WINDOW
project "EternityEngine"
    location "EternityEngine"
    kind "staticLib"
    language "c++"

    targetdir "bin/%{cfg.buildcfg}-%{cfg.buildsystem}-x64/%{prj.name}"
    objdir "bin/%{cfg.buildcfg}-%{cfg.buildsystem}-x64/%{prj.name}"

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.hpp",
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.c",
        "%{prj.name}/**.glsl",
        "%{prj.name}/**.vert",
        "%{prj.name}/**.frag",
    }

    includedirs {
        "%{prj.name}",          -- Allows for Platform
        "%{prj.name}/src/",     -- Removes need for src/ on include for engine 
        "dependencies/include/"

    }
    libdirs{
        "dependencies/lib/glfw",
        "dependencies/lib/vulkan",
    }
    links {
        "vulkan-1.lib",
        "glfw3.lib",
    }
    filter "system:windows"
        cppdialect "c++latest"
        staticruntime "off"
        systemversion "latest"

        defines {
            "WIN32"
        }
    filter "configurations:Debug"
        defines { 
            "_DEBUG"
        }
        symbols "on"
        buildoptions "/MDd"
        
    filter "configurations:Release"
        defines {
            "_DEBUG"
        }
        optimize "on"
        symbols "on"
        buildoptions "/MD"
    filter "configurations:Release"
        defines {
            "NDEBUG"
        }
        optimize "on"
        symbols "off"
        buildoptions "/MD"


project "Eternity"
    location "Eternity"
    kind "consoleapp"
    language "c++"

    targetdir "bin/%{cfg.buildcfg}-%{cfg.buildsystem}-x64/%{prj.name}"
    objdir "bin/%{cfg.buildcfg}-%{cfg.buildsystem}-x64/%{prj.name}"

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.hpp",
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.c",
        "%{prj.name}/**.glsl",
        "%{prj.name}/**.vert",
        "%{prj.name}/**.frag",
    }

    includedirs {
        "%{prj.name}/src/",
        "EternityEngine/src/",
        "EternityEngine/dependencies/include/",
        "dependencies/include/"
    }
    libdirs{
        "dependencies/lib/glfw",
        "dependencies/lib/vulkan",
    }
    links {
        "EternityEngine",
        "vulkan-1.lib",
        "glfw3.lib",
    }
    filter "system:windows"
        cppdialect "c++latest"
        staticruntime "off"
        systemversion "latest"

        defines {
            "WIN32"
        }
    filter "configurations:Debug"
        defines { 
            "_DEBUG"
        }
        symbols "on"
        buildoptions "/MDd"
        
    filter "configurations:Release"
        defines {
            "_DEBUG"
        }
        optimize "on"
        symbols "on"
        buildoptions "/MD"
    filter "configurations:Release"
        defines {
            "NDEBUG"
        }
        optimize "on"
        symbols "off"
        buildoptions "/MD"
