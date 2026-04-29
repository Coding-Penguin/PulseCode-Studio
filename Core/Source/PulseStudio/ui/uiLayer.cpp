#include "pspch.h"
#include "uiLayer.h"
#include "ui/ui.h"
#include "PulseStudio/Log.h"
#include "PulseStudio/Application.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace PulseStudio {

	uiLayer::uiLayer() : Layer("UILayer") {}

	uiLayer::~uiLayer() 
	{
		for (auto* win : m_Windows)
			delete win;
		m_Windows.clear();
		delete menuBar;
	}

	void uiLayer::OnAttach()
	{
		PS_CORE_INFO("uiLayer attached.");

		auto& app = Application::Get();
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();
		int topBarHeight = 30;
		int toolBarHeight = 30;
		int statusBarHeight = 25;

		menuBar = new uiMenuBar();
		menuBar->OnAttach();

		auto* fileExplorer = new uiWindow("FileExplorer");

		m_Windows.push_back(fileExplorer);

		for (auto* win : m_Windows)
		{
			win->OnAttach();
		}
	}

	void uiLayer::OnDetach() 
	{
		for (auto* win : m_Windows)
			win->OnDetach();
		if (menuBar) menuBar->OnDetach();
	}

	void uiLayer::OnUpdate(float deltaTime)
	{
		auto& app = Application::Get();
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();
		if (width == 0 || height == 0) return;

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (menuBar) menuBar->OnUpdate(deltaTime);

		MouseCircle::Get().OnUpdate(deltaTime);

		for (auto* win : m_Windows)
			win->OnUpdate(deltaTime);
	}

	bool uiLayer::OnEvent(Event& event)
	{
		if (menuBar && menuBar->OnEvent(event))
			return true;

		if (MouseCircle::Get().OnEvent(event))
			return true;

		for (auto it = m_Windows.rbegin(); it != m_Windows.rend(); ++it)
		{
			(*it)->OnEvent(event);
			if (event.m_Handled)
				return false;
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
			{
			int topBarHeight = 30, toolBarHeight = 30, statusBarHeight = 25;
			int width = e.GetWidth(), height = e.GetHeight();
			return false;
			});
	}

	void uiLayer::AddWindow(uiWindow* window)
	{
		if (window)
		{
			m_Windows.push_back(window);
			window->OnAttach();
		}
	}

}