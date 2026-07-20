#pragma once

#include "Theme.h"
#include "Channel.h"

namespace PulseCode {

	class Settings
	{
	public:
		Settings();
		~Settings();

		Theme GetTheme() const { return m_Theme; }
		Channel GetChannel() const { return m_Channel; }
	private:
		Theme m_Theme;
		std::string m_ThemeName;
		Channel m_Channel;
		std::string m_ChannelName;
	};

}
