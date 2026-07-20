#pragma once

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"
#include "LayerStack.h"

namespace PulseCode {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(class Layer* layer);
		void PushOverlay(class Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_MainWindow; }

		static bool isFontLoaded() { return s_Instance ? s_Instance->isLoadFont : false; }
		void SetFontLoaded(bool loaded) { if (s_Instance) s_Instance->isLoadFont = loaded; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<class Window> m_MainWindow;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;
		double unsemi_transparency = 0.95f;

		bool isLoadFont = false;
	};

	// To be defined in CLIENT 
	PulseCode::Application* CreateApplication();

}
