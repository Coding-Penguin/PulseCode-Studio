#pragma once
#include "../uiTools/uiWindow.h"
#include "TextBuffer.h"
#include "Cursor.h"
#include "HighLight.h"
#include "EditorView.h"
#include "PulseStudio/Events/KeyEvent.h"
#include "GLFW/glfw3.h"

namespace PulseStudio {

	class CodeEditor
	{
	public:
		CodeEditor();
		virtual ~CodeEditor();

		virtual void OnUpdate(float deltaTime);
		virtual bool OnEvent(Event& event);

		void LoadFile(const std::string& path);
		void SaveFile(const std::string& path);
		void SetText(const std::string& text);
		std::string GetText() const;

		void SetSyntaxMode(const Language& mode);

		void Copy();
		void Cut();
		void Paste();
		bool HasSelection() const { return m_Cursor.HasSelection(); }
		std::string GetSelectedText() const;
		void DeleteSelection();
	private:
		TextBuffer m_Buffer;
		Cursor m_Cursor;
		Highlight m_Highlighter;
		EditorView m_View;

		GLFWcursor* m_ArrowCursor;
		GLFWcursor* m_IBeamCursor;

		bool m_MouseDragSelecting = false;
		CursorPosition m_MouseDragStart;

		void ProcessKeyEvent(KeyPressedEvent& e);
		void ProcessCharEvent(CharEvent& e);
		void ProcessMouseButton(MouseButtonPressedEvent& e);

		void ReplaceSelection(const std::string& text);
	};

}