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

		void SetViewBounds(float x, float y, float w, float h) { m_View->SetBounds(x, y, w, h); }
	private:
		TextBuffer m_Buffer;
		Cursor m_Cursor;
		Highlight m_Highlighter;
		EditorView* m_View;

		GLFWcursor* m_ArrowCursor;
		GLFWcursor* m_IBeamCursor;

		bool m_MouseDragSelecting = false;
		CursorPosition m_MouseDragStart;

		struct UndoAction 
		{
			enum Type { Insert, Delete, InsertNewline, DeleteNewline };
			Type type;
			int line, col;
			std::string data;
			int endLine, endCol;
		};
		std::stack<UndoAction> m_UndoStack;
		std::stack<UndoAction> m_RedoStack;

		void RecordAction(const UndoAction& action);
		void Undo();
		void Redo();
		void ClearRedoStack();

		void RecordInsert(int line, int col, char ch);
		void RecordDelete(int line, int col, char ch);
		void RecordInsertNewline(int line, int col);
		void RecordDeleteNewline(int line, int col, const std::string& nextLineContent);

		void ProcessKeyEvent(KeyPressedEvent& e);
		void ProcessCharEvent(CharEvent& e);
		void ProcessMouseButton(MouseButtonPressedEvent& e);

		void ReplaceSelection(const std::string& text);
	};

}