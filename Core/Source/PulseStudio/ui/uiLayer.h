#pragma once
#include "PulseStudio/Layer.h"
#include "uiTools/uiStatusBar.h"
#include "uiTools/uiShortcutBar.h"
#include <vector>

namespace PulseStudio {

	class uiWindow;
	class OutputWindow;
	class uiTitleBar;
	class uiButton;
	class MouseCircle;
	class CodeEditor;

	class uiLayer : public Layer
	{
	public:
		static uiLayer& Get();

		uiLayer();
		virtual ~uiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual bool OnEvent(Event& event) override;

		void AddWindow(uiWindow* window);

		static bool IsPointOverAnyWindow(float x, float y);

		OutputWindow* GetOutputWindow() const { return m_OutputWindow; }
	private:
		std::vector<uiWindow*> m_Windows;
		uiTitleBar* titleBar = nullptr;
		uiStatusBar* m_StatusBar = nullptr;
		CodeEditor* codeEditor = nullptr;
		uiShortcutBar* m_ShortcutBar = nullptr;
		OutputWindow* m_OutputWindow = nullptr;
	};

}
