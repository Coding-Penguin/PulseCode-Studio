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

		m_StatusBar = new uiStatusBar();
		m_StatusBar->OnAttach();

		titleBar = new uiTitleBar();
		titleBar->OnAttach();

		uiWindow::InitDockSystem(0.0f, 110.0f, width, height - 150.0f);

		auto* fileExplorer = new uiWindow("FileExplorer");
		auto* output = new uiWindow("Output");
		auto* properties = new uiWindow("Properties");
		auto* notifications = new uiWindow("Notifications");
		m_Windows.push_back(fileExplorer);
		m_Windows.push_back(output);
		m_Windows.push_back(properties);
		m_Windows.push_back(notifications);
		uiWindow::DockWindow(fileExplorer, DockRegion::Left);
		uiWindow::DockWindow(output, DockRegion::Bottom);
		uiWindow::DockWindow(properties, DockRegion::Right);
		for (auto* win : m_Windows)
		{
			win->OnAttach();
		}

		m_ShortcutBar = new uiShortcutBar();
		m_ShortcutBar->OnAttach();
		std::vector<ShortcutItem> fileGroup = 
		{
			{ "new", "N", "New File", []() { PS_INFO("New File"); } },
			{ "open", "O", "Open File...", []() { PS_INFO("Open File"); } },
			{ "save", "S", "Save File", []() { PS_INFO("Save File"); } },
			{ "saveall", "SA", "Save All Files", []() { PS_INFO("Save All"); } }
		};
		std::vector<ShortcutItem> editGroup =
		{
			{ "undo", "U", "Undo", []() { PS_INFO("Undo"); }},
			{ "redo", "R", "Redo", []() { PS_INFO("Redo"); } },
			{ "cut", "Ct", "Cut", []() { PS_INFO("Cut"); } },
			{ "copy", "Co", "Copy", []() { PS_INFO("Copy"); } },
			{ "paste", "P", "Paste", []() { PS_INFO("Paste"); } }
		};
		std::vector<ShortcutItem> buildGroup =
		{
			{ "debug", "D", "Debug", []() { PS_INFO("Debug"); } },
			{ "build", "B", "Build", [this]() { PS_INFO("Start Build."); } },
			{ "rebuild", "RB", "Rebuild", []() { PS_INFO("Start Rebuild"); } },
			{ "clean", "Cl", "Clean", []() { PS_INFO("Clean"); } },
			{ "run", "R", "Run", []() { PS_INFO("Run"); } }
		};
		std::vector<ShortcutItem> bookmarkGroup =
		{
			{ "findbookmark", "FB", "Find Bookmark", []() { PS_INFO("Find Bookmark"); } },
			{ "addbookmark", "AB", "Add Bookmark", []() { PS_INFO("Add Bookmark"); } },
			{ "deletebookmark", "DB", "Delete Bookmark", []() { PS_INFO("Delete Bookmark"); } },
			{ "nextbookmark", "NB", "Next Bookmark", []() { PS_INFO("Next Bookmark"); } },
			{ "clearbookmarks", "CB", "Clear Bookmarks", []() { PS_INFO("Clear Bookmarks"); } }
		};

		m_ShortcutBar->AddGroup(fileGroup, true);
		m_ShortcutBar->AddGroup(editGroup, true);
		m_ShortcutBar->AddGroup(buildGroup, true);
		m_ShortcutBar->AddGroup(bookmarkGroup, false);

		codeEditor = new CodeEditor("untitled.cpp");
	}

	void uiLayer::OnDetach() 
	{
		for (auto* win : m_Windows)
			win->OnDetach();
		if (titleBar) titleBar->OnDetach();
		if (m_StatusBar) m_StatusBar->OnDetach();
		if (m_ShortcutBar) m_ShortcutBar->OnDetach();
		delete codeEditor;
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

		MouseCircle::Get().OnUpdate(deltaTime);

		uiWindow::DrawDockAreas();

		double mx, my;
		glfwGetCursorPos(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), &mx, &my);
		uiWindow::DrawDockPanel(mx, my);

		float leftW = uiWindow::GetDynamicLeftWidth();
		float rightW = uiWindow::GetDynamicRightWidth();
		float bottomH = uiWindow::GetDynamicBottomHeight();

		DockRegion preview = uiWindow::GetPreviewRegion();
		if (preview != DockRegion::None)
		{
			float x = 0, y = 0, w = 0, h = 0;
			float mainW = uiWindow::GetMainW();
			float mainH = uiWindow::GetMainH();
			float centerW = mainW - leftW - rightW;
			float centerH = mainH - bottomH;

			switch (preview)
			{
			case DockRegion::Left:
				x = 0; y = 0; w = leftW; h = centerH;
				break;
			case DockRegion::Right:
				x = mainW - rightW; y = 0; w = rightW; h = centerH;
				break;
			case DockRegion::Bottom:
				x = 0; y = mainH - bottomH; w = mainW; h = bottomH;
				break;
			case DockRegion::Center:
				x = leftW; y = 0; w = centerW; h = centerH;
				break;
			default: break;
			}

			x += uiWindow::GetMainX();
			y += uiWindow::GetMainY();

			glEnable(GL_BLEND);
			glColor4f(0.2f, 0.5f, 0.8f, 0.4f);
			glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
			glEnd();
		}

		if (m_ShortcutBar)
		{
			m_ShortcutBar->OnUpdate(deltaTime);
			m_ShortcutBar->Draw();
		}
		if (m_StatusBar) m_StatusBar->OnUpdate(deltaTime);

		if (titleBar) titleBar->OnUpdate(deltaTime);

		if (m_StatusBar) m_StatusBar->SetStatusText("Ready");

		float centerX = uiWindow::GetCenterX();
		float centerY = uiWindow::GetCenterY();
		float centerW = uiWindow::GetCenterW();
		float centerH = uiWindow::GetCenterH();
		codeEditor->SetViewBounds(centerX, centerY, centerW, centerH);
		codeEditor->OnUpdate(deltaTime);
	}

	bool uiLayer::OnEvent(Event& event)
	{
		MouseCircle::Get().OnEvent(event);

		if (titleBar && titleBar->OnEvent(event))
			return true;

		for (auto it = m_Windows.rbegin(); it != m_Windows.rend(); ++it)
		{
			if ((*it)->OnEvent(event))
				return true;
		}

		if (m_ShortcutBar && m_ShortcutBar->OnEvent(event))
			return true;

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
