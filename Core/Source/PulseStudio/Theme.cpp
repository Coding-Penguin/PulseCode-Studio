#include "pspch.h"
#include "Theme.h"
#include "Settings.h"

namespace PulseStudio
{
	Settings s_ThemeSettings;
	Theme ThemeManager::s_CurrentTheme = s_ThemeSettings.GetTheme();
}