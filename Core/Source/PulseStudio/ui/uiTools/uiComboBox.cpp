#include "pspch.h"
#include "uiComboBox.h"
#include "../uiTools/TextRenderer.h"
#include <glad/glad.h>
#include "PulseStudio/Events/MouseEvent.h"

namespace PulseStudio {

	uiComboBox::uiComboBox(float x, float y, float width, float height)
		: m_X(x), m_Y(y), m_W(width), m_H(height)
	{
	}

	uiComboBox::~uiComboBox() {}

	void uiComboBox::SetItems(const std::vector<std::string>& items)
	{
		m_Items = items;
	}

	void uiComboBox::SetSelectedIndex(int index)
	{
		if (index >= 0 && index < (int)m_Items.size())
		{
			m_SelectedIndex = index;
			if (m_Callback) m_Callback(m_SelectedIndex);
		}
	}

	std::string uiComboBox::GetSelectedText() const
	{
		if (m_SelectedIndex >= 0 && m_SelectedIndex < (int)m_Items.size())
			return m_Items[m_SelectedIndex];
		return "";
	}

	void uiComboBox::OnUpdate(float parentX, float parentY)
	{
		Draw(parentX, parentY);
	}

	bool uiComboBox::OnEvent(Event& event, float parentX, float parentY)
	{
		if (event.GetEventType() == EventType::MouseButtonPressed) {
			MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
			float mx = e.GetMouseX(), my = e.GetMouseY();
			float absX = parentX + m_X, absY = parentY + m_Y;

			if (mx >= absX && mx <= absX + m_W && my >= absY && my <= absY + m_H)
			{
				m_Expanded = !m_Expanded;
				return true;
			}

			if (m_Expanded)
			{
				float itemHeight = m_H;
				float listY = absY + m_H;
				for (size_t i = 0; i < m_Items.size(); ++i)
				{
					if (mx >= absX && mx <= absX + m_W && my >= listY && my <= listY + itemHeight) {
						SetSelectedIndex((int)i);
						m_Expanded = false;
						return true;
					}
					listY += itemHeight;
				}

				m_Expanded = false;
				return false;
			}
		}
		return false;
	}

	void uiComboBox::Draw(float parentX, float parentY) const
	{
		float absX = parentX + m_X, absY = parentY + m_Y;

		glColor4f(0.2f, 0.2f, 0.25f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(absX, absY); glVertex2f(absX + m_W, absY);
		glVertex2f(absX + m_W, absY + m_H); glVertex2f(absX, absY + m_H);
		glEnd();

		std::string display = GetSelectedText();
		float textX = absX + 5;
		float textY = absY + (m_H - 18) / 2;
		TextRenderer::Get().DrawText(display, textX, textY, 1.0f, 1.0f, 1.0f, 1.0f);

		float arrowX = absX + m_W - 15;
		float arrowY = absY + m_H * 0.35f;
		glBegin(GL_TRIANGLES);
		glVertex2f(arrowX, arrowY);
		glVertex2f(arrowX + 10, arrowY);
		glVertex2f(arrowX + 5, arrowY + 8);
		glEnd();

		if (m_Expanded)
		{
			float itemHeight = m_H;
			float listY = absY + m_H;
			for (size_t i = 0; i < m_Items.size(); ++i)
			{
				glColor4f(0.15f, 0.15f, 0.18f, 1.0f);
				glBegin(GL_QUADS);
				glVertex2f(absX, listY); glVertex2f(absX + m_W, listY);
				glVertex2f(absX + m_W, listY + itemHeight); glVertex2f(absX, listY + itemHeight);
				glEnd();
				TextRenderer::Get().DrawText(m_Items[i], textX, listY + (itemHeight - 18) / 2, 1, 1, 1, 1);
				listY += itemHeight;
			}
		}
	}

}
