#include "pspch.h"
#include "EditorTabManager.h"
#include "PulseCode/Application.h"
#include "PulseCode/Log.h"
#include <glad/glad.h>

namespace PulseCode {

	EditorTabManager::EditorTabManager()
	{
	}

	EditorTabManager::~EditorTabManager()
	{
	}

	void EditorTabManager::SetBounds(float x, float y, float w, float h)
	{
		m_TabBarX = x;
		m_TabBarY = y;
		m_TabBarW = w;
		m_TabBarH = 30.0f;

		m_EditorX = x;
		m_EditorY = y + m_TabBarH;
		m_EditorW = w;
		m_EditorH = h - m_TabBarH;

		if (m_ActiveTabIndex >= 0 && m_ActiveTabIndex < (int)m_Tabs.size()) 
		{
			auto& tab = m_Tabs[m_ActiveTabIndex];
			if (tab.editor)
			{
				tab.editor->OnUpdate(0.0f);
			}
		}
	}

	void EditorTabManager::OnUpdate(float deltaTime)
	{
		if (m_Tabs.empty() || m_ActiveTabIndex < 0) return;
		if (m_ActiveTabIndex >= (int)m_Tabs.size()) return;

		auto& tab = m_Tabs[m_ActiveTabIndex];
		if (tab.editor)
		{
			tab.editor->OnUpdate(deltaTime);
		}
	}

	bool EditorTabManager::OnEvent(Event& event)
	{
		if (event.GetEventType() == EventType::MouseScrolled)
		{
			MouseScrolledEvent& e = (MouseScrolledEvent&)event;
			float mx = e.GetMouseX(), my = e.GetMouseY();

			float viewX = CodeEditor::GetView()->GetX();
			float viewY = CodeEditor::GetView()->GetY();
			float viewW = CodeEditor::GetView()->GetWidth();
			float viewH = CodeEditor::GetView()->GetHeight();

			if (mx >= viewX && mx <= viewX + viewW &&
				my >= viewY && my <= viewY + viewH)
			{
				CodeEditor::GetView()->HandleScroll(e.GetXOffset() * 50.0f, -e.GetYOffset() * 50.0f);
				return true;
			}
			return false;
		}

		if (m_ActiveTabIndex >= 0 && m_ActiveTabIndex < (int)m_Tabs.size())
		{
			auto& tab = m_Tabs[m_ActiveTabIndex];
			if (tab.editor && tab.editor->OnEvent(event))
			{
				return true;
			}
		}

		if (event.GetEventType() == EventType::MouseButtonPressed)
		{
			MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
			return OnMouseButton(e, e.GetMouseX(), e.GetMouseY());
		}
		if (event.GetEventType() == EventType::MouseMoved)
		{
			MouseMovedEvent& e = (MouseMovedEvent&)event;
			return OnMouseMove(e, e.GetX(), e.GetY());
		};
		return false;
	}

	void EditorTabManager::Draw()
	{
		if (m_Tabs.empty()) return;

		DrawTabBar();

		if (m_ActiveTabIndex >= 0 && m_ActiveTabIndex < (int)m_Tabs.size())
		{
			auto& tab = m_Tabs[m_ActiveTabIndex];
		}
	}

	void EditorTabManager::DrawTabBar()
	{
		if (m_Tabs.empty()) return;

		float tabX = m_TabBarX;
		float tabY = m_TabBarY;
		float tabH = m_TabBarH;
		float maxWidth = m_TabBarW;

		for (size_t i = 0; i < m_Tabs.size(); ++i)
		{
			auto& tab = m_Tabs[i];
			float tabW = GetTabWidth(tab.title) + 30.0f;
			if (tabX + tabW > m_TabBarX + m_TabBarW)
			{
				break;
			}
			DrawTab(i, tabX, tabY, tabW, tabH, tab.isActive);
			tabX += tabW;
		}
	}

	void EditorTabManager::DrawTab(int index, float x, float y, float w, float h, bool active)
	{
		if (ThemeManager::IsDarkTheme())
		{
			if (active)
			{
				glColor4f(0.25f, 0.25f, 0.28f, 0.7f);
			}
			else
			{
				glColor4f(0.18f, 0.18f, 0.20f, 0.7f);
			}
		}
		else
		{
			if (active)
			{
				glColor4f(0.8f, 0.8f, 0.83f, 0.7f);
			}
			else
			{
				glColor4f(0.9f, 0.9f, 0.93f, 0.7f);
			}
		}

		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		glColor4f(0.3f, 0.3f, 0.35f, 1.0f);
		glBegin(GL_LINES);
		glVertex2f(x + w - 1, y + 2);
		glVertex2f(x + w - 1, y + h - 2);
		glEnd();

		float textX = x + 8;
		float textY = y + (h - 18) / 2;
		float r = 0.9f, g = 0.9f, b = 0.9f;
		if (!active)
		{
			r = g = b = 0.7f;
		}

		TextRenderer::Get().DrawText(m_Tabs[index].title, textX, textY, r, g, b, 1.0f);

		float closeX = x + w - 20;
		float closeY = y + (h - 16) / 2;
		glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
		glBegin(GL_LINES);
		glVertex2f(closeX, closeY);
		glVertex2f(closeX + 12, closeY + 12);
		glVertex2f(closeX + 12, closeY);
		glVertex2f(closeX, closeY + 12);
		glEnd();
	}

	float EditorTabManager::GetTabWidth(const std::string& title) const
	{
		float textWidth = TextRenderer::Get().GetTextWidth(title);
		return textWidth + 30.0f;
	}

	void EditorTabManager::OpenFile(const std::string& filepath)
	{
		for (size_t i = 0; i < m_Tabs.size(); ++i)
		{
			if (m_Tabs[i].filepath == filepath)
			{
				SwitchToTab(i);
				return;
			}
		}

		Tab newTab;
		newTab.filepath = filepath;
		newTab.title = GetFileName(filepath);
		newTab.editor = std::make_unique<CodeEditor>();
		newTab.editor->LoadFile(filepath);
		m_Tabs.push_back(std::move(newTab));

		SwitchToTab((int)m_Tabs.size() - 1);
	}

	void EditorTabManager::CloseTab(int index)
	{
		if (index < 0 || index >= (int)m_Tabs.size()) return;

		if (index == m_ActiveTabIndex)
		{
			int newActive = (index > 0) ? index - 1 : (m_Tabs.size() > 1 ? 0 : -1);
			m_Tabs.erase(m_Tabs.begin() + index);
			if (newActive >= 0 && newActive < (int)m_Tabs.size())
			{
				SwitchToTab(newActive);
			}
			else
			{
				m_ActiveTabIndex = -1;
			}
		}
		else
		{
			m_Tabs.erase(m_Tabs.begin() + index);
			if (m_ActiveTabIndex > index)
			{
				m_ActiveTabIndex--;
			}
		}
	}

	void EditorTabManager::CloseCurrentTab()
	{
		if (m_ActiveTabIndex >= 0)
		{
			CloseTab(m_ActiveTabIndex);
		}
	}

	void EditorTabManager::SwitchToTab(int index)
	{
		if (index < 0 || index >= (int)m_Tabs.size()) return;

		for (auto& tab : m_Tabs)
		{
			tab.isActive = false;
		}
		m_Tabs[index].isActive = true;
		m_ActiveTabIndex = index;
	}

	CodeEditor* EditorTabManager::GetActiveEditor() const
	{
		if (m_ActiveTabIndex >= 0 && m_ActiveTabIndex < (int)m_Tabs.size())
		{
			return m_Tabs[m_ActiveTabIndex].editor.get();
		}
		return nullptr;
	}

	bool EditorTabManager::OnMouseButton(MouseButtonPressedEvent& e, float mx, float my)
	{
		if (my < m_TabBarY || my > m_TabBarY + m_TabBarH) return false;
		if (m_Tabs.empty()) return false;

		float tabX = m_TabBarX;
		for (size_t i = 0; i < m_Tabs.size(); ++i)
		{
			float tabW = GetTabWidth(m_Tabs[i].title) + 30.0f;
			if (mx >= tabX && mx <= tabX + tabW)
			{
				float closeX = tabX + tabW - 20;
				float closeY = m_TabBarY + (m_TabBarH - 16) / 2;
				if (mx >= closeX && mx <= closeX + 12 && my >= closeY && my <= closeY + 12)
				{
					CloseTab(i);
					return true;
				}
				else
				{
					SwitchToTab(i);
					return true;
				}
			}
			tabX += tabW;
		}
		return false;
	}

	bool EditorTabManager::OnMouseMove(MouseMovedEvent& e, float mx, float my)
	{
		return false;
	}

	std::string EditorTabManager::GetFileName(const std::string& path) const
	{
		std::filesystem::path p(path);
		return p.filename().string();
	}

}