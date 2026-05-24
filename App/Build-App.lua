project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp" }

   includedirs
   {
        "Source",
        "../Core/Source",
        "../Core/vendor/GLFW/include",
        "../Core/vendor/Glad/include",
        "../Core/vendor/glm",
        "../Core/vendor/stb_image",
        "../Core/vendor/miniaudio",
        "../Core/vendor/spdlog/include",
        "../Core/vendor/json/include",
        "../Core/vendor"
   }

   links { "Core" }

   targetdir ("../Binaries/" .. OutputDir)
   objdir ("../Binaries-Intermediates/" .. OutputDir)

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS", "PS_PLATFORM_WINDOWS" }

   filter "system:linux"
       defines { "LINUX", "PS_PLATFORM_LINUX" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"