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

		for (auto* win : m_Windows)
		{
			win->OnAttach();
		}

		m_ShortcutBar = new uiShortcutBar();
		m_ShortcutBar->OnAttach();
		std::vector<ShortcutItem> fileGroup = 
		{
			{"new", "N", []() { PS_INFO("New File"); }},
			{"open", "O", []() { PS_INFO("Open File"); }},
			{"save", "S", []() { PS_INFO("Save File"); }},
			{"saveall", "SA", []() { PS_INFO("Save All"); }}
		};
		std::vector<ShortcutItem> editGroup =
		{
			{"undo", "U", []() { PS_INFO("Undo"); }},
			{"redo", "R", []() { PS_INFO("Redo"); }},
			{"cut", "Ct", []() { PS_INFO("Cut"); }},
			{"copy", "Co", []() { PS_INFO("Copy"); }},
			{"paste", "P", []() { PS_INFO("Paste"); }}
		};

		m_ShortcutBar->AddGroup(fileGroup, true);
		m_ShortcutBar->AddGroup(editGroup, false);

		codeEditor = new CodeEditor();
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

		if (codeEditor) codeEditor->OnUpdate(deltaTime);
		
		if (titleBar) titleBar->OnUpdate(deltaTime);
		if (m_ShortcutBar)
		{
			m_ShortcutBar->OnUpdate(deltaTime);
			m_ShortcutBar->Draw();
		}
		if (m_StatusBar) m_StatusBar->OnUpdate(deltaTime);

		MouseCircle::Get().OnUpdate(deltaTime);

		if (m_StatusBar) m_StatusBar->SetStatusText("Ready");

		for (auto* win : m_Windows)
			win->OnUpdate(deltaTime);
	}

	bool uiLayer::OnEvent(Event& event)
	{
		if (titleBar && titleBar->OnEvent(event))
			return true;

		if (m_ShortcutBar && m_ShortcutBar->OnEvent(event))
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
