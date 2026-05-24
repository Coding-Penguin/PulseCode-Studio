workspace "Pulse Studio IDE"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "App"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/utf-8" }

OutputDir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"

group "Core"
	include "Core/Build-Core.lua"
group "Application"
   include "App/Build-App.lua"
group "Vendor"
   include "Core/vendor/GLFW"
   include "Core/vendor/Glad"
   include "Core/vendor/glm"
   include "Core/vendor/stb_image"
group ""