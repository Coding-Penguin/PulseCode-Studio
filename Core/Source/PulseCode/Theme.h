#pragma once

namespace PulseCode
{

	enum class Theme // 6 items
	{
		// Light
		Light = 0,
		Cool_Breeze,
		Icy_Mint,
		// Dark
		Dark,
		Cool_Slate,
		Moonlight
	};

	class ThemeManager
	{
	public:
		static Theme GetCurrentTheme() { return s_CurrentTheme; }
		static void SetTheme(Theme theme) { s_CurrentTheme = theme; }
		static bool IsDarkTheme() { return (s_CurrentTheme == Theme::Dark || s_CurrentTheme == Theme::Cool_Slate || s_CurrentTheme == Theme::Moonlight); }
	private:
		ThemeManager() = default;

		ThemeManager(const ThemeManager&) = delete;
		ThemeManager& operator=(const ThemeManager&) = delete;
		ThemeManager(ThemeManager&&) = delete;
		ThemeManager& operator=(ThemeManager&&) = delete;
		ThemeManager& operator=(const ThemeManager&&) = delete;

		static Theme s_CurrentTheme;
	};

}
