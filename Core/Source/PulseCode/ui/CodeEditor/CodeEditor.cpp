#include "pspch.h"
#include "CodeEditor.h"
#include "PulseCode/Log.h"
#include <fstream>
#include <sstream>
#include "PulseCode/Application.h"
#include "EditorView.h"

#include <GLFW/glfw3.h>

namespace PulseCode {

	CodeEditor::CodeEditor(const std::string& path)
	{
		m_Buffer.LoadFromFile(path);
		m_Cursor.MoveTo(0, 0);

		m_View = &EditorView::Get();

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		m_ArrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		m_IBeamCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	}

	CodeEditor::~CodeEditor()
	{
		glfwDestroyCursor(m_ArrowCursor);
		glfwDestroyCursor(m_IBeamCursor);
	}

	void CodeEditor::OnUpdate(float deltaTime)
	{
		m_View->Render(m_Buffer, m_Cursor, m_Highlighter, deltaTime);
	}

	bool CodeEditor::OnEvent(Event& event)
	{
		if (event.GetEventType() == EventType::KeyPressed)
		{
			ProcessKeyEvent((KeyPressedEvent&)event);
			return true;
		}
		if (event.GetEventType() == EventType::Char)
		{
			ProcessCharEvent((CharEvent&)event);
			return true;
		}
		if (event.GetEventType() == EventType::MouseButtonPressed)
		{
			MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
			float mx = e.GetMouseX(), my = e.GetMouseY();
			if (m_View->OnMouseButton(e, mx, my))
			{
				return true;
			}

			if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
			{
				bool inContent = (mx >= m_View->GetX() && mx <= m_View->GetX() + m_View->GetWidth() && my >= m_View->GetY() && my <= m_View->GetY() + m_View->GetHeight());
				if (inContent)
				{
					CursorPosition pos = m_View->ScreenToTextPosition(mx, my, m_Buffer);
					m_Cursor.SetPosition(pos.line, pos.col);
					m_Cursor.StartSelection();
					m_MouseDragSelecting = true;
					m_MouseDragStart = pos;
					return true;
				}
			}
			return false;
		}
		if (event.GetEventType() == EventType::MouseMoved)
		{
			MouseMovedEvent& e = (MouseMovedEvent&)event;
			float mx = e.GetX(), my = e.GetY();
			if (m_View->OnMouseMove(e, mx, my))
			{
				return true;
			}

			float viewX = m_View->GetX();
			float viewY = m_View->GetY();
			float viewW = m_View->GetWidth();
			float viewH = m_View->GetHeight();

			bool inContent = (mx >= viewX && mx <= viewX + viewW && my >= viewY && my <= viewY + viewH);

			GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			if (inContent)
			{
				glfwSetCursor(window, m_IBeamCursor);
			}
			else
			{
				glfwSetCursor(window, m_ArrowCursor);
			}

			if (m_MouseDragSelecting)
			{
				CursorPosition pos = m_View->ScreenToTextPosition(mx, my, m_Buffer);
				m_Cursor.SetPosition(pos.line, pos.col);
				return true;
			}

			return false;
		}
		if (event.GetEventType() == EventType::MouseButtonReleased)
		{
			MouseButtonReleasedEvent& e = (MouseButtonReleasedEvent&)event;
			m_View->OnMouseRelease();
			if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT && m_MouseDragSelecting)
			{
				m_MouseDragSelecting = false;
				return true;
			}
		}
		if (event.GetEventType() == EventType::MouseScrolled)
		{
			MouseScrolledEvent& e = (MouseScrolledEvent&)event;
			float mx = e.GetMouseX(), my = e.GetMouseY();
			float viewX = m_View->GetX();
			float viewY = m_View->GetY();
			float viewW = m_View->GetWidth();
			float viewH = m_View->GetHeight();

			if (mx >= viewX && mx <= viewX + viewW &&
				my >= viewY && my <= viewY + viewH)
			{
				m_View->HandleScroll(e.GetXOffset() * 30.0f, -e.GetYOffset() * 30.0f);
				return true;
			}
			return false;
		}
		return false;
	}

	void CodeEditor::LoadFile(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			PS_CORE_ERROR("Failed to open file: {}", path);
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		SetText(buffer.str());

		std::string extension = GetFileExtension(path);
		if (extension == "cpp" || extension == "c" || extension == "h")
		{
			m_Highlighter.SetLanguage(Language::CPP);
		}
		else if (extension == "py")
		{
			m_Highlighter.SetLanguage(Language::Python);
		}
		else if (extension == "java")
		{
			m_Highlighter.SetLanguage(Language::Java);
		}
		else if (extension == "cs")
		{
			m_Highlighter.SetLanguage(Language::CSharp);
		}
		else if (extension == "md")
		{
			m_Highlighter.SetLanguage(Language::Markdown);
		}
		else if (extension == "json")
		{
			m_Highlighter.SetLanguage(Language::JSON);
		}
		else if (extension == "lua")
		{
			m_Highlighter.SetLanguage(Language::Lua);
		}

		PS_CORE_INFO("Loaded file: {}", path);
	}

	void CodeEditor::SaveFile(const std::string& path)
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			PS_CORE_ERROR("Failed to save file: {}", path);
			return;
		}
		file << GetText();
		file.close();
		PS_CORE_INFO("Saved file: {}", path);
	}

	std::string CodeEditor::GetText() const
	{
		return m_Buffer.GetString();
	}

	void CodeEditor::SetSyntaxMode(const Language& mode)
	{
		m_Highlighter.SetLanguage(mode);
	}

	void CodeEditor::ProcessKeyEvent(KeyPressedEvent& e)
	{
		int key = e.GetKeyCode();
		int mods = e.GetMods();

		if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_C)
		{
			Copy();
			return;
		}
		if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_X)
		{
			Cut();
			return;
		}
		if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_V)
		{
			Paste();
			return;
		}
		if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_Z)
		{
			Undo();
			return;
		}
		if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_Y)
		{
			Redo();
			return;
		}
		if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_A)
		{
			m_Cursor.SetPosition(0, 0);
			m_Cursor.StartSelection();
			m_Cursor.SetPosition(m_Buffer.GetLineCount() - 1, m_Buffer.GetLineLength(m_Buffer.GetLineCount() - 1));
			return;
		}

		if (key == GLFW_KEY_LEFT)
		{
			m_Cursor.Move(0, -1, m_Buffer, (mods & GLFW_MOD_SHIFT) != 0);
		}
		else if (key == GLFW_KEY_RIGHT)
		{
			m_Cursor.Move(0, 1, m_Buffer, (mods & GLFW_MOD_SHIFT) != 0);
		}
		else if (key == GLFW_KEY_UP)
		{
			m_Cursor.Move(-1, 0, m_Buffer, (mods & GLFW_MOD_SHIFT) != 0);
		}
		else if (key == GLFW_KEY_DOWN)
		{
			m_Cursor.Move(1, 0, m_Buffer, (mods & GLFW_MOD_SHIFT) != 0);
		}
		else if (key == GLFW_KEY_HOME)
		{
			if (mods & GLFW_MOD_CONTROL)
				m_Cursor.MoveToTop();
			else
				m_Cursor.MoveToLineStart(m_Buffer);
		}
		else if (key == GLFW_KEY_END)
		{
			if (mods & GLFW_MOD_CONTROL)
				m_Cursor.MoveToBottom(m_Buffer);
			else
				m_Cursor.MoveToLineEnd(m_Buffer);
		}
		else if (key == GLFW_KEY_BACKSPACE)
		{
			CursorPosition pos = m_Cursor.GetPosition();
			if (m_Cursor.HasSelection())
			{
				DeleteSelection();
			}
			else
			{
				if (pos.col > 0)
				{
					char deleted = m_Buffer.GetLine(pos.line)[pos.col - 1];
					RecordDelete(pos.line, pos.col - 1, deleted);
					m_Buffer.DeleteChar(pos.line, pos.col - 1);
					m_Cursor.Move(0, -1, m_Buffer, mods & GLFW_MOD_SHIFT);
				}
				else if (pos.line > 0)
				{
					std::string nextLine = m_Buffer.GetLine(pos.line);
					RecordDeleteNewline(pos.line - 1, m_Buffer.GetLineLength(pos.line - 1), nextLine);
					int prevLine = pos.line - 1;
					int prevCol = m_Buffer.GetLineLength(prevLine);
					std::string currentLine = m_Buffer.GetLine(pos.line);
					m_Buffer.DeleteRange(prevLine, prevCol, pos.line, 0);
					m_Buffer.SetLine(prevLine, m_Buffer.GetLine(prevLine) + currentLine);
					m_Cursor.MoveTo(prevLine, prevCol);
				}
			}
		}
		else if (key == GLFW_KEY_DELETE)
		{
			CursorPosition pos = m_Cursor.GetPosition();
			if (pos.col < m_Buffer.GetLineLength(pos.line))
			{
				char deleted = m_Buffer.GetLine(pos.line)[pos.col];
				RecordDelete(pos.line, pos.col, deleted);
				m_Buffer.DeleteChar(pos.line, pos.col);
			}
			else if (pos.line + 1 < m_Buffer.GetLineCount())
			{
				std::string nextLine = m_Buffer.GetLine(pos.line + 1);
				RecordDeleteNewline(pos.line, pos.col, nextLine);
				m_Buffer.SetLine(pos.line, m_Buffer.GetLine(pos.line) + nextLine);
				m_Buffer.DeleteRange(pos.line + 1, 0, pos.line + 2, 0);
			}
		}
		else if (key == GLFW_KEY_ENTER)
		{
			m_Cursor.EndSelection();
			CursorPosition pos = m_Cursor.GetPosition();
			std::string currentLine = m_Buffer.GetLine(pos.line);
			RecordInsertNewline(pos.line, pos.col);

			int indentSize = 0;
			while (indentSize < (int)currentLine.size() && (currentLine[indentSize] == ' ' || currentLine[indentSize] == '\t'))
			{
				indentSize++;
			}
			std::string indent = currentLine.substr(0, indentSize);

			bool shouldIncrease = false;
			if (pos.col == (int)currentLine.size() && !currentLine.empty() && currentLine.back() == '{')
			{
				shouldIncrease = true;
			}

			std::string leftPart = currentLine.substr(0, pos.col);
			std::string rightPart = currentLine.substr(pos.col);
			m_Buffer.SetLine(pos.line, leftPart);

			std::string newIndent = indent;
			if (shouldIncrease)
			{
				newIndent += "    ";
			}
			std::string newLine = newIndent + rightPart;
			m_Buffer.InsertLine(pos.line + 1, newLine);

			m_Cursor.MoveTo(pos.line + 1, (int)newIndent.size());
		}
		else if (key == GLFW_KEY_TAB)
		{
			for (int i = 0; i < 4; ++i)
				ProcessCharEvent(*new CharEvent(' '));
		}
	}

	void CodeEditor::ProcessCharEvent(CharEvent& e)
	{
		unsigned int ch = e.GetCharCode();
		if (ch >= 32 && ch <= 126)
		{
			if (m_Cursor.HasSelection()) 
			{
				DeleteSelection();
			}
			CursorPosition pos = m_Cursor.GetPosition();
			RecordInsert(pos.line, pos.col, (char)ch);
			if (ch == '(' || ch == '[' || ch == '{')
			{
				char right = (ch == '(' ? ')' : (ch == '[' ? ']' : '}'));
				const std::string& line = m_Buffer.GetLine(pos.line);
				bool nextIsMatching = (pos.col < (int)line.size() && line[pos.col] == right);
				m_Buffer.InsertChar(pos.line, pos.col, (char)ch);
				if (!nextIsMatching)
				{
					m_Buffer.InsertChar(pos.line, pos.col + 1, right);
				}
				m_Cursor.SetPosition(pos.line, pos.col + 1);
			}
			else
			{
				m_Buffer.InsertChar(pos.line, pos.col, (char)ch);
				m_Cursor.Move(0, 1, m_Buffer, false);
			}
			m_Cursor.EndSelection();
		}
	}

	void CodeEditor::ProcessMouseButton(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() != GLFW_MOUSE_BUTTON_LEFT) return;
		float mx = e.GetMouseX();
		float my = e.GetMouseY();
		CursorPosition pos = m_View->ScreenToTextPosition(mx, my, m_Buffer);
		m_Cursor.SetPosition(pos.line, pos.col);
	}

	std::string CodeEditor::GetSelectedText() const
	{
		if (!m_Cursor.HasSelection()) return "";
		int startLine, startCol, endLine, endCol;
		m_Cursor.GetSelectionRange(startLine, startCol, endLine, endCol);
		if (startLine == endLine)
		{
			const std::string& line = m_Buffer.GetLine(startLine);
			return line.substr(startCol, endCol - startCol);
		}
		else
		{
			std::string result;

			const std::string& firstLine = m_Buffer.GetLine(startLine);
			result += firstLine.substr(startCol);

			for (int l = startLine + 1; l < endLine; ++l)
			{
				result += '\n';
				result += m_Buffer.GetLine(l);
			}

			result += '\n';
			const std::string& lastLine = m_Buffer.GetLine(endLine);
			result += lastLine.substr(0, endCol);
			return result;
		}
	}

	void CodeEditor::Copy()
	{
		bool hasSel = m_Cursor.HasSelection();
		std::string text = GetSelectedText();
		if (!text.empty())
		{
			GLFWwindow* win = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			glfwSetClipboardString(win, text.c_str());
		}
	}

	void CodeEditor::Cut()
	{
		Copy();
		DeleteSelection();
	}

	void CodeEditor::Paste()
	{
		GLFWwindow* win = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		const char* clipText = glfwGetClipboardString(win);
		PS_CORE_INFO("Paste: clipboard text = '{}'", clipText ? clipText : "(null)");
		if (clipText)
		{
			ReplaceSelection(clipText);
		}
	}

	void CodeEditor::DeleteSelection()
	{
		if (!m_Cursor.HasSelection()) return;
		int startLine, startCol, endLine, endCol;
		m_Cursor.GetSelectionRange(startLine, startCol, endLine, endCol);
		if (startLine == endLine)
		{
			m_Buffer.DeleteRange(startLine, startCol, startLine, endCol);
			m_Cursor.SetPosition(startLine, startCol);
		}
		else
		{
			m_Buffer.DeleteRange(startLine, startCol, endLine, endCol);
			m_Cursor.SetPosition(startLine, startCol);
		}
		m_Cursor.EndSelection();
	}

	void CodeEditor::ReplaceSelection(const std::string& text)
	{
		DeleteSelection();

		std::istringstream stream(text);
		std::string line;
		std::vector<std::string> lines;
		while (std::getline(stream, line))
		{
			lines.push_back(line);
		}
		if (lines.empty()) return;
		CursorPosition pos = m_Cursor.GetPosition();

		for (char c : lines[0])
		{
			m_Buffer.InsertChar(pos.line, pos.col, c);
			pos.col++;
		}
		m_Cursor.SetPosition(pos.line, pos.col);

		for (size_t i = 1; i < lines.size(); ++i)
		{
			m_Buffer.InsertNewline(pos.line, pos.col);
			m_Cursor.MoveTo(pos.line + 1, 0);
			pos = m_Cursor.GetPosition();
			for (char c : lines[i])
			{
				m_Buffer.InsertChar(pos.line, pos.col, c);
				pos.col++;
			}
			m_Cursor.SetPosition(pos.line, pos.col);
		}
	}

	void CodeEditor::ClearRedoStack()
	{
		while (!m_RedoStack.empty()) m_RedoStack.pop();
	}

	void CodeEditor::RecordAction(const UndoAction& action)
	{
		m_UndoStack.push(action);
		ClearRedoStack();
	}

	void CodeEditor::RecordInsert(int line, int col, char ch)
	{
		UndoAction action;
		action.type = UndoAction::Insert;
		action.line = line;
		action.col = col;
		action.data = std::string(1, ch);
		RecordAction(action);
	}

	void CodeEditor::RecordDelete(int line, int col, char ch)
	{
		UndoAction action;
		action.type = UndoAction::Delete;
		action.line = line;
		action.col = col;
		action.data = std::string(1, ch);
		RecordAction(action);
	}

	void CodeEditor::RecordInsertNewline(int line, int col)
	{
		UndoAction action;
		action.type = UndoAction::InsertNewline;
		action.line = line;
		action.col = col;
		RecordAction(action);
	}

	void CodeEditor::RecordDeleteNewline(int line, int col, const std::string& nextLineContent)
	{
		UndoAction action;
		action.type = UndoAction::DeleteNewline;
		action.line = line;
		action.col = col;
		action.data = nextLineContent;
		RecordAction(action);
	}

	void CodeEditor::Undo()
	{
		if (m_UndoStack.empty()) return;
		UndoAction action = m_UndoStack.top();
		m_UndoStack.pop();

		switch (action.type)
		{
		case UndoAction::Insert:
			m_Buffer.DeleteChar(action.line, action.col);
			m_Cursor.SetPosition(action.line, action.col);
			break;
		case UndoAction::Delete:
			m_Buffer.InsertChar(action.line, action.col, action.data[0]);
			m_Cursor.SetPosition(action.line, action.col + 1);
			break;
		case UndoAction::InsertNewline:
		{
			std::string nextLine = m_Buffer.GetLine(action.line + 1);
			m_Buffer.SetLine(action.line, m_Buffer.GetLine(action.line) + nextLine);
			m_Buffer.DeleteLine(action.line + 1);
			m_Cursor.SetPosition(action.line, action.col);
		}
		break;
		case UndoAction::DeleteNewline:
		{
			std::string currentLine = m_Buffer.GetLine(action.line);
			std::string left = currentLine.substr(0, action.col);
			std::string right = currentLine.substr(action.col);
			m_Buffer.SetLine(action.line, left);
			m_Buffer.InsertLine(action.line + 1, right);
			m_Cursor.SetPosition(action.line + 1, 0);
		}
		break;
		}
		m_RedoStack.push(action);
	}

	void CodeEditor::Redo() 
	{
		if (m_RedoStack.empty()) return;
		UndoAction action = m_RedoStack.top();
		m_RedoStack.pop();

		switch (action.type)
		{
		case UndoAction::Insert:
			m_Buffer.InsertChar(action.line, action.col, action.data[0]);
			m_Cursor.SetPosition(action.line, action.col + 1);
			break;
		case UndoAction::Delete:
			m_Buffer.DeleteChar(action.line, action.col);
			m_Cursor.SetPosition(action.line, action.col);
			break;
		case UndoAction::InsertNewline:
			m_Buffer.InsertNewline(action.line, action.col);
			m_Cursor.SetPosition(action.line + 1, 0);
			break;
		case UndoAction::DeleteNewline:
		{
			std::string nextLine = m_Buffer.GetLine(action.line + 1);
			m_Buffer.SetLine(action.line, m_Buffer.GetLine(action.line) + nextLine);
			m_Buffer.DeleteLine(action.line + 1);
			m_Cursor.SetPosition(action.line, action.col);
		}
		break;
		}
		m_UndoStack.push(action);
	}

	std::string CodeEditor::GetFileExtension(const std::string& path) const
	{
		size_t pos = path.find_last_of('.');
		if (pos != std::string::npos)
		{
			return path.substr(pos + 1);
		}
		return "";
	}

	void CodeEditor::SetText(const std::string& text)
	{
		m_Buffer.LoadFromString(text);
		m_Cursor.MoveTo(0, 0);
		m_View->HandleScroll(0, 0);
		m_View->EnsureCursorVisible(m_Cursor, m_Buffer);
	}

}
