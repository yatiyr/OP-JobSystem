project "Op-JobSystem"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
    	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    	files
    	{
        	"src/**.h",
        	"src/**.cpp",
        	"src/**.hpp",
		"src/**.cpp",
    	}

	includedirs
	{
        	"src"
	}


	filter "system:windows"
		systemversion "latest"

		defines
		{
			"OP_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "OP_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "OP_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OP_DIST"
		runtime "Release"
		optimize "on"