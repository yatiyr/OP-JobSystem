-- require "cmake"

workspace "OP-JobSystem"
    architecture "x86_64"
    startproject "Op-JobSystem"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
    include "Op-JobSystem"
group ""
        