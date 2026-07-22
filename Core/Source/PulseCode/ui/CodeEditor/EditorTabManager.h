#pragma once
#include <vector>
#include <string>
#include <functional>
#include "CodeEditor.h"
#include "EditorView.h"
#include "../uiTools/uiButton.h"

namespace PulseCode {

	class EditorTabManager
	{
	public:
		EditorTabManager();
		~EditorTabManager();

		void SetBounds(float x, float y, float w, float h);
		void OnUpdate(float deltaTime);
		bool OnEvent(Event& event);

		void OpenFile(const std::string& filepath);

		void CloseTab(int index);
		void CloseCurrentTab();

		void SwitchToTab(int index);

		CodeEditor* GetActiveEditor() const;

		void Draw();
	private:
		struct Tab
		{
			std::string filepath;
			std::string title;
			std::unique_ptr<CodeEditor> editor;
			bool isActive = false;
		};

		std::vector<Tab> m_Tabs;
		int m_ActiveTabIndex = -1;

		float m_TabBarX, m_TabBarY, m_TabBarW, m_TabBarH;
		float m_EditorX, m_EditorY, m_EditorW, m_EditorH;

		void DrawTabBar();

		void DrawTab(int index, float x, float y, float w, float h, bool active);

		float GetTabWidth(const std::string& title) const;

		bool OnMouseButton(MouseButtonPressedEvent& e, float mx, float my);
		bool OnMouseMove(MouseMovedEvent& e, float mx, float my);

		std::string GetFileName(const std::string& path) const;
	};

}