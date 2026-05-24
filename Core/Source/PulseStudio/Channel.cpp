#include "pspch.h"
#include "Channel.h"

namespace PulseStudio
{
	Settings s_ChannelSettings;
	Channel ChannelManager::s_Channel = s_ChannelSettings.GetChannel();
}