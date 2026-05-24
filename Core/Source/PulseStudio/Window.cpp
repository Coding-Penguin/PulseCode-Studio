#include "pspch.h"

#include "Window.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Linux/LinuxWindow.h"

#include "Log.h"

namespace PulseStudio {

    Window* Window::Create(const WindowProps& props)
    {
#ifdef PS_PLATFORM_WINDOWS
        PS_TRACE("Creating Windows window...");
        return new WindowsWindow(props);
#elif defined(PS_PLATFORM_LINUX)
        PS_TRACE("Creating Linux window...");
        return new LinuxWindow(props);
#else
        PS_CORE_ERROR("Pulse Studio IDE unsupported the platform!");
        return nullptr;
#endif
    }

}