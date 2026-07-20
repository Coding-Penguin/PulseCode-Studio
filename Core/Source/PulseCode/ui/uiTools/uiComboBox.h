#pragma once
#include <string>
#include <vector>
#include <functional>
#include "PulseCode/Events/Event.h"

namespace PulseCode {

	class uiComboBox
	{
	public:
		uiComboBox(float x, float y, float width, float height);
		~uiComboBox();

		void OnUpdate(float parentX, float parentY);
		bool OnEvent(Event& event, float parentX, float parentY);
		void SetItems(const std::vector<std::string>& items);
		void SetSelectedIndex(int index);
		int GetSelectedIndex() const { return m_SelectedIndex; }
		std::string GetSelectedText() const;
		void SetOnSelectionChanged(std::function<void(int)> callback) { m_Callback = callback; }
		void Draw(float parentX, float parentY) const;

	private:
		float m_X, m_Y, m_W, m_H;
		std::vector<std::string> m_Items;
		int m_SelectedIndex = 0;
		bool m_Expanded = false;
		std::function<void(int)> m_Callback;
	};

}
