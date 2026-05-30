#include "pspch.h"
#include "CodeEditor.h"
#include "PulseStudio/Log.h"
#include <fstream>
#include <sstream>
#include "PulseStudio/Application.h"

#include <GLFW/glfw3.h>

namespace PulseStudio {

	CodeEditor::CodeEditor()
	{
		m_Buffer.LoadFromString("");
		m_Cursor.MoveTo(0, 0);
		m_Highlighter.SetLanguage(Language::CPP);

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
		float contentX = 0.0f;
		float contentY = 55.0f;
		Application& app = Application::Get();
		float contentW = app.GetWindow().GetWidth();
		float contentH = app.GetWindow().GetHeight() - contentY;
		m_View.SetBounds(contentX, contentY, contentW, contentH);

		m_View.Render(m_Buffer, m_Cursor, m_Highlighter, deltaTime);
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
			if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
			{
				bool inContent = (mx >= m_View.GetX() && mx <= m_View.GetX() + m_View.GetWidth() && my >= m_View.GetY() + 30 && my <= m_View.GetY() + m_View.GetHeight());
				if (inContent)
				{
					CursorPosition pos = m_View.ScreenToTextPosition(mx, my, m_Buffer);
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

			float viewX = m_View.GetX();
			float viewY = m_View.GetY();
			float viewW = m_View.GetWidth();
			float viewH = m_View.GetHeight();

			bool inContent = (mx >= viewX && mx <= viewX + viewW && my >= viewY && my <= viewY + viewH);

			GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			if (inContent)
				glfwSetCursor(window, m_IBeamCursor);
			else
				glfwSetCursor(window, m_ArrowCursor);

			if (m_MouseDragSelecting)
			{
				CursorPosition pos = m_View.ScreenToTextPosition(mx, my, m_Buffer);
				m_Cursor.SetPosition(pos.line, pos.col);
				return true;
			}

			return false;
		}
		if (event.GetEventType() == EventType::MouseButtonReleased)
		{
			MouseButtonReleasedEvent& e = (MouseButtonReleasedEvent&)event;
			if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT && m_MouseDragSelecting)
			{
				m_MouseDragSelecting = false;
				m_Cursor.EndSelection();
				return true;
			}
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

	void CodeEditor::SetText(const std::string& text)
	{
		m_Buffer.LoadFromString(text);
		m_Cursor.MoveTo(0, 0);
		m_View.HandleScroll(0, 0);
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
					m_Buffer.DeleteChar(pos.line, pos.col - 1);
					m_Cursor.Move(0, -1, m_Buffer, mods & GLFW_MOD_SHIFT);
				}
				else if (pos.line > 0)
				{
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
				m_Buffer.DeleteChar(pos.line, pos.col);
			}
			else if (pos.line + 1 < m_Buffer.GetLineCount())
			{
				std::string nextLine = m_Buffer.GetLine(pos.line + 1);
				m_Buffer.SetLine(pos.line, m_Buffer.GetLine(pos.line) + nextLine);
				m_Buffer.DeleteRange(pos.line + 1, 0, pos.line + 2, 0);
			}
		}
		else if (key == GLFW_KEY_ENTER)
		{
			CursorPosition pos = m_Cursor.GetPosition();
			m_Buffer.InsertNewline(pos.line, pos.col);
			m_Cursor.MoveTo(pos.line + 1, 0);
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
			KeyPressedEvent& e = *new KeyPressedEvent(ch, 0);
			int mods = e.GetMods();
			CursorPosition pos = m_Cursor.GetPosition();
			m_Buffer.InsertChar(pos.line, pos.col, (char)ch);
			m_Cursor.Move(0, 1, m_Buffer, mods & GLFW_MOD_SHIFT);
			m_Cursor.EndSelection();
		}
	}

	void CodeEditor::ProcessMouseButton(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() != GLFW_MOUSE_BUTTON_LEFT) return;
		float mx = e.GetMouseX();
		float my = e.GetMouseY();
		CursorPosition pos = m_View.ScreenToTextPosition(mx, my, m_Buffer);
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

}
