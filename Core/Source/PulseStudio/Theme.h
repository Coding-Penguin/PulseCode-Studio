#pragma once

namespace PulseStudio
{

	enum class Theme // 10 items
	{
		// Light
		Light = 0,
		Cool_Breeze,
		Icy_Mint,
		Sand,
		Ice,
		// Dark
		Dark,
		Cool_Slate,
		Moonlight,
		Hacker,
		Grape
	};

	class ThemeManager
	{
	public:
		static Theme GetCurrentTheme() { return s_CurrentTheme; }
		static void SetTheme(Theme theme) { s_CurrentTheme = theme; }
		static bool IsDarkTheme() { return (s_CurrentTheme == Theme::Dark || s_CurrentTheme == Theme::Cool_Slate || s_CurrentTheme == Theme::Moonlight || s_CurrentTheme == Theme::Hacker || s_CurrentTheme == Theme::Grape); }
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
