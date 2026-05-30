#include "pspch.h"
#include "uiLayer.h"
#include "uiTools/ui.h"
#include "PulseStudio/Log.h"
#include "PulseStudio/Application.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "CodeEditor/CodeEditor.h"

namespace PulseStudio {

	uiLayer& uiLayer::Get()
	{
		static uiLayer instance;
		return instance;
	}

	uiLayer::uiLayer() : Layer("UILayer") {}

	uiLayer::~uiLayer() 
	{
		for (auto* win : m_Windows)
			delete win;
		m_Windows.clear();
		delete titleBar;
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

		titleBar = new uiTitleBar();
		titleBar->OnAttach();

		m_StatusBar = new uiStatusBar();
		m_StatusBar->OnAttach();

		//auto* fileExplorer = new uiWindow("FileExplorer");
		//m_Windows.push_back(fileExplorer);

		codeEditor = new CodeEditor();

		for (auto* win : m_Windows)
		{
			win->OnAttach();
		}
	}

	void uiLayer::OnDetach() 
	{
		for (auto* win : m_Windows)
			win->OnDetach();
		if (titleBar) titleBar->OnDetach();
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

		if (titleBar) titleBar->OnUpdate(deltaTime);
		if (m_StatusBar) m_StatusBar->OnUpdate(deltaTime);

		codeEditor->OnUpdate(deltaTime);

		MouseCircle::Get().OnUpdate(deltaTime);

		if (m_StatusBar) m_StatusBar->SetStatusText("Ready");

		for (auto* win : m_Windows)
			win->OnUpdate(deltaTime);
	}

	bool uiLayer::OnEvent(Event& event)
	{
		if (titleBar && titleBar->OnEvent(event))
			return true; 

		if (MouseCircle::Get().OnEvent(event))
			return true;

		for (auto it = m_Windows.rbegin(); it != m_Windows.rend(); ++it)
		{
			if ((*it)->OnEvent(event))
				return true;
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
			{
			int topBarHeight = 30, toolBarHeight = 30, statusBarHeight = 25;
			int width = e.GetWidth(), height = e.GetHeight();
			return false;
			});

		codeEditor->OnEvent(event);
		if (event.GetEventType() == EventType::MouseMoved)
		{
			MouseMovedEvent& e = (MouseMovedEvent&)event;
			float mx = e.GetX(), my = e.GetY();
			if (event.GetEventType() == EventType::MouseMoved)
			{
				if (codeEditor && codeEditor->OnEvent(event))
					return true;
			}
			return true; 
		}

		return false;
	}

	void uiLayer::AddWindow(uiWindow* window)
	{
		if (window)
		{
			m_Windows.push_back(window);
			window->OnAttach();
		}
	}

	bool uiLayer::IsPointOverAnyWindow(float x, float y)
	{
		auto& instance = Get();
		for (auto* win : instance.m_Windows)
		{
			if (win->IsVisible())
			{
				float wx = win->GetX();
				float wy = win->GetY();
				float ww = win->GetWidth();
				float wh = win->GetHeight();

				if (x >= wx && x <= wx + ww && y >= wy && y <= wy + wh)
				{
					return true;
				}
			}
		}
		return false;
	}

}
