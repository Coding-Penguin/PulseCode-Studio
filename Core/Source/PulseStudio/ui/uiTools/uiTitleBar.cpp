#include "pspch.h"
#include "uiTitleBar.h"
#include "PhotoRenderer.h"
#include "uiButton.h"
#include "PulseStudio/Application.h"
#include "PulseStudio/Events/Event.h"
#ifdef PS_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Search.h"

namespace PulseStudio
{

	uiTitleBar::uiTitleBar()
	{
		float btnWidth = 40.0f;
		float btnHeight = 40.0f;
		float y = 0;
		float rightMargin = 0;
		float startX = Application::Get().GetWindow().GetWidth() - 3 * btnWidth - rightMargin;

		m_MinimizeRect = { startX, y, btnWidth, btnHeight };
		m_MaximizeRect = { startX + btnWidth, y, btnWidth, btnHeight };
		m_CloseRect = { startX + 2 * btnWidth, y, btnWidth, btnHeight };
	}
	uiTitleBar::~uiTitleBar()
	{
	}

	void uiTitleBar::OnAttach()
	{
		m_Logo.reset(new PhotoRenderer());
		m_Logo->LoadFromFile("H:/Projects/CppProject/Pulse-Studio/Core/Resources/Images/System.png");
		PS_INFO("Logo loaded: {0}", m_Logo->IsLoaded());

		float x = 40, y = 10;
		auto addTitleButton = [&](const std::string& text, float width)
			{
				auto btn = std::make_unique<uiButton>(text, x, y, width, 30, ButtonStyles::NoBackgroundOrLine);
				btn->SetCallback([=]() { PS_INFO("Clicked \"{}\"", text); });
				m_Buttons.push_back(std::move(btn));
				x += width + 10;
			};

		addTitleButton("File", 70);
		addTitleButton("Edit", 70);
		addTitleButton("View", 70);
		addTitleButton("Project", 90);
		addTitleButton("Build", 75);
		addTitleButton("Debug", 75);
		addTitleButton("Tools", 75);
		addTitleButton("Help", 70);
		x += 30;

		auto addSearchButton = [&](const std::string& text, float width)
			{
				auto btn = std::make_unique<uiButton>(text, x, y, width, 30, ButtonStyles::NoBackgroundOrLine);
				btn->SetCallback([=]() { PS_INFO("Clicked \"{}\"", text); });
				m_Buttons.push_back(std::move(btn));
				x += width + 10;
			};
		addSearchButton("Search...", 130);
	}

	void uiTitleBar::OnDetach()
	{
		m_Logo->Unload();
	}

	void uiTitleBar::OnUpdate(float deltaTime)
	{
		if (m_Logo && m_Logo->IsLoaded())
		{
			m_Logo->Draw(10, 5, 20, 20);
		}

		for (auto& btn : m_Buttons) 
		{
			btn->OnUpdate(0, 0, true);
		}

		auto& app = Application::Get();
		int width = app.GetWindow().GetWidth();
		UpdateWindowButtonsPosition(width);

		GLFWwindow* win = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		bool isMaximized = glfwGetWindowAttrib(win, GLFW_MAXIMIZED) == GLFW_TRUE;

		if (ChannelManager::GetChannel() == Channel::Preview)
		{
			DrawMinimizeButton(m_MinimizeRect.x, m_MinimizeRect.y, m_MinimizeRect.w, m_MinimizeRect.h, m_MinimizeHovered);
			DrawMaximizeButton(m_MaximizeRect.x, m_MaximizeRect.y, m_MaximizeRect.w, m_MaximizeRect.h, m_MaximizeHovered, isMaximized);
			DrawCloseButton(m_CloseRect.x, m_CloseRect.y, m_CloseRect.w, m_CloseRect.h, m_CloseHovered);
		}
	}

	bool uiTitleBar::OnEvent(Event &event)
	{
		for (auto& btn : m_Buttons)
		{
			if (btn->OnEvent(event, 0, 0, true))
				return true;
		}

		MouseMovedEvent& e = (MouseMovedEvent&)event;
		float mx = e.GetX(), my = e.GetY();

		if (event.GetEventType() == EventType::MouseMoved)
		{
			if (ChannelManager::GetChannel() == Channel::Preview)
			{
				if (m_DraggingMainWindow)
				{
					float dx = mx - m_DragStartX;
					float dy = my - m_DragStartY;

					m_AccumulatedDragX += dx;
					m_AccumulatedDragY += dy;

					if (std::abs(m_AccumulatedDragX) >= 1.0f || std::abs(m_AccumulatedDragY) >= 1.0f)
					{
						int newX = m_WindowStartX + (int)(m_AccumulatedDragX);
						int newY = m_WindowStartY + (int)(m_AccumulatedDragY);
						auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
						glfwSetWindowPos(window, newX, newY);

						m_WindowStartX = newX;
						m_WindowStartY = newY;
						m_AccumulatedDragX -= (int)m_AccumulatedDragX;
						m_AccumulatedDragY -= (int)m_AccumulatedDragY;
					}

					m_DragStartX = mx;
					m_DragStartY = my;
					return true;
				}
			}

			auto updateHover = [&](ButtonRect& rect, bool& hover)
				{
					hover = (mx >= rect.x && mx <= rect.x + rect.w && my >= rect.y && my <= rect.y + rect.h);
				};
			updateHover(m_MinimizeRect, m_MinimizeHovered);
			updateHover(m_MaximizeRect, m_MaximizeHovered);
			updateHover(m_CloseRect, m_CloseHovered);
			return false;
			}
		else if (event.GetEventType() == EventType::MouseButtonPressed)
		{
			MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
			if (e.GetMouseButton() != GLFW_MOUSE_BUTTON_LEFT) return false;
			float mx = e.GetMouseX(), my = e.GetMouseY();

			if (ChannelManager::GetChannel() == Channel::Preview)
			{
				if (mx >= m_MinimizeRect.x && mx <= m_MinimizeRect.x + m_MinimizeRect.w &&
					my >= m_MinimizeRect.y && my <= m_MinimizeRect.y + m_MinimizeRect.h)
				{
					auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
					glfwIconifyWindow(window);
					return true;
				}

				if (mx >= m_MaximizeRect.x && mx <= m_MaximizeRect.x + m_MaximizeRect.w &&
					my >= m_MaximizeRect.y && my <= m_MaximizeRect.y + m_MaximizeRect.h)
				{
					auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
					if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
						glfwRestoreWindow(window);
					else
						glfwMaximizeWindow(window);
					return true;
				}

				if (mx >= m_CloseRect.x && mx <= m_CloseRect.x + m_CloseRect.w &&
					my >= m_CloseRect.y && my <= m_CloseRect.y + m_CloseRect.h)
				{
					auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
					glfwSetWindowShouldClose(window, GLFW_TRUE);
					PS_CORE_INFO("Close button clicked!");
					return true;
				}
			}

			if (my < 30)
			{
				bool hitButton = false;

				for (auto& btn : m_Buttons)
				{
					float bx = btn->GetX(), by = btn->GetY(), bw = btn->GetWidth(), bh = btn->GetHeight();
					if (mx >= bx && mx <= bx + bw && my >= by && my <= by + bh)
					{
						hitButton = true;
						break;
					}
				}

				if (!hitButton && ((mx >= m_MinimizeRect.x && mx <= m_MinimizeRect.x + m_MinimizeRect.w &&
					my >= m_MinimizeRect.y && my <= m_MinimizeRect.y + m_MinimizeRect.h) ||
					(mx >= m_MaximizeRect.x && mx <= m_MaximizeRect.x + m_MaximizeRect.w &&
						my >= m_MaximizeRect.y && my <= m_MaximizeRect.y + m_MaximizeRect.h) ||
					(mx >= m_CloseRect.x && mx <= m_CloseRect.x + m_CloseRect.w &&
						my >= m_CloseRect.y && my <= m_CloseRect.y + m_CloseRect.h)))
				{
					hitButton = true;
				}

				if (!hitButton)
				{
					auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
					glfwGetWindowPos(window, &m_WindowStartX, &m_WindowStartY);
					m_DragStartX = mx;
					m_DragStartY = my;
					m_DraggingMainWindow = true;
					return true;
				}
			}
		}
		else if (event.GetEventType() == EventType::MouseButtonReleased)
		{
			MouseButtonReleasedEvent& e = (MouseButtonReleasedEvent&)event;
			if (e.GetMouseButton() != GLFW_MOUSE_BUTTON_LEFT) return false;
			if (m_DraggingMainWindow)
			{
				m_DraggingMainWindow = false;
				return true;
			}
			return false;
		}

		return false;
	}

	void uiTitleBar::Draw()
	{
		if (m_Logo && m_Logo->IsLoaded())
		{
			m_Logo->Draw(100, 100, 200, 200);
		}

		PS_INFO("Drawed logo.");
	}

	void uiTitleBar::UpdateWindowButtonsPosition(int windowWidth)
	{		
		float btnWidth = 40.0f;
		float btnHeight = 40.0f;
		float y = 0;
		float rightMargin = 0;
		float startX = windowWidth - 3 * btnWidth - rightMargin;

		m_MinimizeRect = { startX, y, btnWidth, btnHeight };
		m_MaximizeRect = { startX + btnWidth, y, btnWidth, btnHeight };
		m_CloseRect = { startX + 2 * btnWidth, y, btnWidth, btnHeight };
	}

	void uiTitleBar::OnWindowResize(int width, int height)
	{
		UpdateWindowButtonsPosition(width);
	}

	void uiTitleBar::DrawMinimizeButton(float x, float y, float w, float h, bool hovered)
	{
		if (hovered)
			glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);

		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		float lineY = y + h / 2;
		float left = x + w * 0.2f;
		float right = x + w * 0.8f;

		if (ThemeManager::IsDarkTheme())
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(left, lineY);
		glVertex2f(right, lineY);
		glEnd();
	}

	void uiTitleBar::DrawMaximizeButton(float x, float y, float w, float h, bool hovered, bool isMaximized)
	{
		if (hovered)
			glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y); glVertex2f(x + w, y);
		glVertex2f(x + w, y + h); glVertex2f(x, y + h);
		glEnd();

		if (ThemeManager::IsDarkTheme())
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

		glLineWidth(2.0f);

		float margin;
		if (isMaximized)
		{
			margin = w * 0.23f;
		}
		else
		{
			margin = w * 0.2f;
		}
		float left = x + margin;
		float right = x + w - margin;
		float top = y + margin;
		float bottom = y + h - margin;

		if (isMaximized)
		{
			float offsetOuterX = 1.7f;
			float offsetOuterY = -1.7f;
			float outerX = left + offsetOuterX;
			float outerY = top + offsetOuterY;
			float outerW = right - left;
			float outerH = bottom - top;

			glBegin(GL_LINES);

			glVertex2f(outerX, outerY);
			glVertex2f(outerX + outerW, outerY);

			glVertex2f(outerX + outerW, outerY);
			glVertex2f(outerX + outerW, outerY + outerH);
			glEnd();

			float offsetInnerX = -3.4f;
			float offsetInnerY = 3.4f;
			float innerX = outerX + offsetInnerX;
			float innerY = outerY + offsetInnerY;

			glBegin(GL_LINE_LOOP);
			glVertex2f(innerX, innerY);
			glVertex2f(innerX + outerW, innerY);
			glVertex2f(innerX + outerW, innerY + outerH);
			glVertex2f(innerX, innerY + outerH);
			glEnd();
		}
		else
		{
			glBegin(GL_LINE_LOOP);
			glVertex2f(left, top);
			glVertex2f(right, top);
			glVertex2f(right, bottom);
			glVertex2f(left, bottom);
			glEnd();
		}
	}

	void uiTitleBar::DrawCloseButton(float x, float y, float w, float h, bool hovered)
	{
		if (hovered)
			glColor4f(0.8f, 0.2f, 0.2f, 1.0f);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);

		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		float margin = w * 0.2f;
		float left = x + margin;
		float right = x + w - margin;
		float top = y + margin;
		float bottom = y + h - margin;

		if (ThemeManager::IsDarkTheme())
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(left, top);
		glVertex2f(right, bottom);
		glVertex2f(right, top);
		glVertex2f(left, bottom);
		glEnd();
	}

}
