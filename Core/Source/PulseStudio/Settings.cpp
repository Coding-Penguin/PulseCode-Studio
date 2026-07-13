#include "pspch.h"
#include "Settings.h"

#include "json/include/nlohmann/json.hpp"

namespace PulseStudio {

	Settings::Settings()
	{
		nlohmann::json json;
		std::ifstream file("config/settings.json");
		if (!file.is_open())
		{
			std::ofstream out("config/settings.json");
			json["theme"] = "Dark";
			out << json;
		}
		else
		{
			try
			{
				file >> json;
			}
			catch (const std::exception& e)
			{
				PS_CORE_ERROR("Failed to parse JSON: {}", e.what());
				return;
			}

			m_ThemeName = json["theme"].get<std::string>();
			m_ChannelName = json["channel"].get<std::string>();

			if (m_ThemeName == "Light")
			{
				m_Theme = Theme::Light;
			}
			else if (m_ThemeName == "Cool_Breeze")
			{
				m_Theme = Theme::Cool_Breeze;
			}
			else if (m_ThemeName == "Icy_Mint")
			{
				m_Theme = Theme::Icy_Mint;
			}
			else if (m_ThemeName == "Dark")
			{
				m_Theme = Theme::Dark;
			}
			else if (m_ThemeName == "Cool_Slate")
			{
				m_Theme = Theme::Cool_Slate;
			}
			else if (m_ThemeName == "Moonlight")
			{
				m_Theme = Theme::Moonlight;
			}
			else
			{
				PS_CORE_ERROR("Theme in settings file was break! Use the dark theme.");
				std::ofstream file("config/settings.json");
				m_Theme = Theme::Dark;
				json["theme"] = "Dark";
				file << json.dump(4);
				file.close();
			}

			if (m_ChannelName == "Current")
			{
				m_Channel = Channel::Current;
			}
			else if (m_ChannelName == "Preview")
			{
				m_Channel = Channel::Preview;
			}
			else
			{
				PS_CORE_ERROR("Channel in settings file was break! Use the Current Channel.");
				std::ofstream file("config/settings.json");
				m_Channel = Channel::Current;
				json["channel"] = "Current";
				file << json.dump(4);
				file.close();
			}
		}

		file.close();
	}

	Settings::~Settings()
	{
	}

}
