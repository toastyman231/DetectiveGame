engineLocation = os.getenv("BASED_ENGINE_HOME")
if not engineLocation then
    error("BASED_ENGINE_HOME environment variable is not set. Please try restarting your terminal, or configure it to point to the engine directory.")
end

include (engineLocation .. "\\dependencies.lua")
externalBuild = true

workspace "DetectiveGame"
    startproject "DetectiveGame"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

tdir = "bin/%{cfg.buildcfg}/%{prj.name}"
odir = "bin-obj/%{cfg.buildcfg}/%{prj.name}"

group "Engine"
    include (engineLocation .. "\\based")
group ""

group "Dependencies"
    include (engineLocation .. "\\external\\jolt")
    include (engineLocation .. "\\external\\glad")
    include (engineLocation .. "\\external\\yaml-cpp")
group ""

group "Projects"
    project "DetectiveGame"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"
        links "based"

        targetdir(tdir)
        objdir(odir)

        files 
        {
            "src/**.h",
            "src/**.cpp"
        }

        includedirs
        {
            engineLocation .. "\\based\\include",
            engineLocation .. "\\%{externals.spdlog}",
            engineLocation .. "\\%{externals.rmlui}",
            engineLocation .. "\\%{externals.tracy}",
            engineLocation .. "\\%{externals.yaml_cpp}",
            engineLocation .. "\\%{externals.jolt}"
        }

        include "Plugins/FMOD"

        flags
        {
            "FatalWarnings"
        }

        defines
        {
            "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING"
        }

        postbuildcommands
        {
            "python3 " .. string.sub(path.getabsolute("%{prj.name}"), 1, string.len(path.getabsolute("%{prj.name}")) - (string.len("%{prj.name}") + 1)) .. "/postbuild.py config=%{cfg.buildcfg} prj=%{prj.name}"
        }   

        filter {"system:windows", "configurations:Debug"}
            systemversion "latest"

            defines
            {
                "BASED_PLATFORM_WINDOWS",
                "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
                "JPH_DEBUG_RENDERER"
            }

        filter {"system:windows", "configurations:Release"}
            systemversion "latest"

            defines
            {
                "BASED_PLATFORM_WINDOWS"
            }

        filter {"system:macosx", "configurations:*"}
            xcodebuildsettings
            {
                ["MACOSX_DEPLOYMENT_TARGET"] = "10.15",
                ["UseModernBuildSystem"] = "NO"
            }

            defines
            {
                "BASED_PLATFORM_MAC"
            }

            abspath = path.getabsolute("%{externals.maclibs}")
            linkoptions {"-F " .. abspath}

        filter {"system:linux", "configurations:*"}
            defines
            {
                "BASED_PLATFORM_LINUX"
            }

        filter {"configurations:Debug"}
            defines
            {
                "BASED_CONFIG_DEBUG"
            }
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines
            {
                "BASED_CONFIG_RELEASE"
            }
            runtime "Release"
            symbols "off"
            optimize "on"
group""