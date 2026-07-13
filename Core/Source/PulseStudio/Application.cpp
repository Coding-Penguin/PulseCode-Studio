#include "pspch.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"
#include "Window.h"
#include "ui/uiTools/uiWindow.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glad/glad.h>

#ifdef PS_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#else define(PS_PLATFORM_LINUX)
#include "Platform/Linux/LinuxWindow.h"
#endif

#include "Input.h"

namespace PulseStudio {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		PS_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		Log::Init();
		PS_CORE_INFO("Initilized log!");

		ThemeManager::SetTheme(Theme::Moonlight);

		WindowProps props("Pulse-Studio Integrated Development Environment", 1720, 1000);
		m_MainWindow = std::unique_ptr<Window>(Window::Create(props));
		m_MainWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Input::Init();

		if (!isFontLoaded())
		{
			PS_CORE_INFO("Loading font...");
			TextRenderer::Get().LoadFont("H:/Projects/CppProject/Pulse-Studio/Core/Resources/Fonts/Ubuntu-L.ttf", 24.0f);
			SetFontLoaded(true);
			PS_CORE_INFO("Font loaded successfully.");
		}
	}

	Application::~Application()
	{
		TextRenderer::Get().Unload();

		PS_INFO("Application destructor called.");
		PS_CORE_WARN("Shutting down Pulse Studio...");
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		PS_CORE_TRACE(e.ToString());
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
	}
	
	static bool SetGLFWColor()
	{
		if (ThemeManager::GetCurrentTheme() == Theme::Dark)
		{
			glClearColor(0.01f, 0.01f, 0.05f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			return true;
		}
		else if (ThemeManager::GetCurrentTheme() == Theme::Light)
		{
			glClearColor(0.95f, 0.95f, 1.0f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			return true;
		}
		else if (ThemeManager::GetCurrentTheme() == Theme::Cool_Breeze)
		{
			glClearColor(0.8f, 0.9f, 0.95f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			return true;
		}
		else if (ThemeManager::GetCurrentTheme() == Theme::Cool_Slate)
		{
			glClearColor(0.110f, 0.208f, 0.306f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			return true;
		}
		else if (ThemeManager::GetCurrentTheme() == Theme::Icy_Mint)
		{
			glClearColor(0.8f, 0.9f, 0.85f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			return true;
		}
		else if (ThemeManager::GetCurrentTheme() == Theme::Moonlight)
		{
			glClearColor(0.05f, 0.1f, 0.17f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			return true;
		}
		else
		{
			PS_ERROR("Unknow this theme!");
			return false;
		}
	}

	void Application::Run()
	{
		PS_TRACE("Pulse Studio initialized and running.");

		float lastTime = (float)glfwGetTime();
		do 
		{
			float currentTime = (float)glfwGetTime();
			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			SetGLFWColor();

			for (Layer* layer : m_LayerStack)
				if (layer)
					layer->OnUpdate(deltaTime);

			if (m_MainWindow)
			{
				m_MainWindow->SetUnsemi_transparency(unsemi_transparency);
				m_MainWindow->OnUpdate();
			}
			else
			{
				PS_CORE_ERROR("Main window is null!");
				m_Running = false;
			}
		} while (m_Running && !glfwWindowShouldClose(static_cast<GLFWwindow*>(m_MainWindow->GetNativeWindow())));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) 
	{
		PS_WARN("Window close event received. Shutting down Application...");
		m_Running = false;
		return true;
	}

}
