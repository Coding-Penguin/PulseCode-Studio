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
		delete m_StatusBar;
		delete m_ShortcutBar;
		delete codeEditor;
	}

	void uiLayer::OnAttach()
	{
		PS_CORE_INFO("uiLayer attached.");

		auto& app = Application::Get();
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();
		int topBarHeight = 30;
		int toolBarHeight = 30;
		int statusBarHeight = 35;

		titleBar = new uiTitleBar();
		titleBar->OnAttach();

		m_StatusBar = new uiStatusBar();
		m_StatusBar->OnAttach();

		uiWindow::InitDockSystem(0.0f, 110.0f, width, height - 145.0f);

		auto* fileExplorer = new uiWindow("FileExplorer");
		uiWindow::DockWindow(fileExplorer, DockRegion::Left);
		m_Windows.push_back(fileExplorer);

		for (auto* win : m_Windows)
		{
			win->OnAttach();
		}

		m_ShortcutBar = new uiShortcutBar();
		m_ShortcutBar->OnAttach();
		std::vector<ShortcutItem> fileGroup = 
		{
			{"new", "N", "New File", []() { PS_INFO("New File"); }},
			{"open", "O", "Open File...", []() { PS_INFO("Open File"); }},
			{"save", "S", "Save File", []() { PS_INFO("Save File"); }},
			{"saveall", "SA", "Save All Files", []() { PS_INFO("Save All"); }}
		};
		std::vector<ShortcutItem> editGroup =
		{
			{"undo", "U", "Undo", []() { PS_INFO("Undo"); }},
			{"redo", "R", "Redo", []() { PS_INFO("Redo"); }},
			{"cut", "Ct", "Cut", []() { PS_INFO("Cut"); }},
			{"copy", "Co", "Copy", []() { PS_INFO("Copy"); }},
			{"paste", "P", "Paste", []() { PS_INFO("Paste"); }}
		};
		std::vector<ShortcutItem> buildGroup =
		{
			{"debug", "D", "Debug", []() { PS_INFO("Debug"); }},
			{"build", "B", "Build", []() { PS_INFO("Build"); }},
			{"rebuild", "RB", "Rebuild", []() { PS_INFO("Rebuild"); }},
			{"clean", "Cl", "Clean", []() { PS_INFO("Clean"); }},
			{"run", "R", "Run", []() { PS_INFO("Run"); }}
		};
		std::vector<ShortcutItem> bookmarkGroup =
		{
			{"findbookmark", "FB", "Find Bookmark", []() { PS_INFO("Find Bookmark"); }},
			{"addbookmark", "AB", "Add Bookmark", []() { PS_INFO("Add Bookmark"); }},
			{"deletebookmark", "DB", "Delete Bookmark", []() { PS_INFO("Delete Bookmark"); }},
			{"nextbookmark", "NB", "Next Bookmark", []() { PS_INFO("Next Bookmark"); }},
			{"clearbookmarks", "CB", "Clear Bookmarks", []() { PS_INFO("Clear Bookmarks"); }}
		};

		m_ShortcutBar->AddGroup(fileGroup, true);
		m_ShortcutBar->AddGroup(editGroup, true);
		m_ShortcutBar->AddGroup(buildGroup, true);
		m_ShortcutBar->AddGroup(bookmarkGroup, false);

		codeEditor = new CodeEditor();
	}

	void uiLayer::OnDetach() 
	{
		for (auto* win : m_Windows)
			win->OnDetach();
		if (titleBar) titleBar->OnDetach();
		if (m_StatusBar) m_StatusBar->OnDetach();
		if (m_ShortcutBar) m_ShortcutBar->OnDetach();
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
		if (m_ShortcutBar)
		{
			m_ShortcutBar->OnUpdate(deltaTime);
			m_ShortcutBar->Draw();
		}
		if (m_StatusBar) m_StatusBar->OnUpdate(deltaTime);

		MouseCircle::Get().OnUpdate(deltaTime);

		if (m_StatusBar) m_StatusBar->SetStatusText("Ready");

		if (codeEditor) codeEditor->OnUpdate(deltaTime);

		for (auto* win : m_Windows)
			win->OnUpdate(deltaTime);

		uiWindow::DrawDockAreas();

		float leftW = width * 0.2f,
			rightW = width * 0.2f,
			bottomH = height * 0.3f;
		float mainW = (float)width, mainH = (float)height;
		float centerW = mainW - leftW - rightW;
		float centerH = mainH - bottomH;
		float yOffset = 110.0f;
		codeEditor->SetViewBounds(leftW, yOffset, centerW, centerH);
		codeEditor->OnUpdate(deltaTime);
	}

	bool uiLayer::OnEvent(Event& event)
	{
		MouseCircle::Get().OnEvent(event);

		if (titleBar && titleBar->OnEvent(event))
			return true;

		if (m_ShortcutBar && m_ShortcutBar->OnEvent(event))
			return true;

		for (auto it = m_Windows.rbegin(); it != m_Windows.rend(); ++it)
		{
			if ((*it)->OnEvent(event))
				return true;
		}

		if (codeEditor) return codeEditor->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
			{
			int width = e.GetWidth(), height = e.GetHeight();
			uiWindow::OnWindowResize(width, height - 145.0f);

			Application& app = Application::Get();
			float leftW = app.GetWindow().GetWidth() * 0.2f,
				rightW = app.GetWindow().GetWidth() * 0.2f,
				bottomH = app.GetWindow().GetHeight() * 0.3f;
			float mainW = (float)width;
			float mainH = (float)height;
			float centerW = mainW - leftW - rightW;
			float centerH = mainH - bottomH;
			float yOffset = 110.0f;

			EditorView::Get().SetBounds(leftW, yOffset, centerW, centerH);

			return false;
			});

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
