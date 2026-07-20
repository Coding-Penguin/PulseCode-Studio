#pragma once

#include "pspch.h"

#include "Event.h"

namespace PulseCode {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount, int mods)
			: KeyEvent(keycode), m_RepeatCount(repeatCount), m_Mods(mods) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }
		inline int GetMods() const { return m_Mods; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: Key: " << (char)m_KeyCode << ", KeyCode: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
		int m_Mods;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class CharEvent : public Event
	{
	public:
		CharEvent(unsigned int codepoint)
			: m_Codepoint(codepoint)
		{
		}

		unsigned int GetCharCode() const { return m_Codepoint; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "CharEvent: " << m_Codepoint;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Char)
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	private:
		unsigned int m_Codepoint;
	};


}
