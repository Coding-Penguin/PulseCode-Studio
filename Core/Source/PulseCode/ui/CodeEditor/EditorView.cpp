#include "pspch.h"
#include "EditorView.h"
#include "../uiTools/TextRenderer.h"
#include "PulseCode/Application.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cmath>

#include "PulseCode/FontManager.h"
#include "HighLight.h"
#include "PulseCode/Events/MouseEvent.h"

namespace PulseCode
{

	EditorView::EditorView()
	{
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/PulseCode-Studio/Core/Resources/Fonts/CascadiaCode.ttf", m_FontSize, FontStyle::Regular);
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/PulseCode-Studio/Core/Resources/Fonts/CascadiaCode-Bold.ttf", m_FontSize, FontStyle::Bold);
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/PulseCode-Studio/Core/Resources/Fonts/CascadiaCode-Italic.ttf", m_FontSize, FontStyle::Italic);
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/PulseCode-Studio/Core/Resources/Fonts/CascadiaCode-BoldItalic.ttf", m_FontSize, FontStyle::BoldItalic);

		m_RegularFont = FontManager::Get().GetFont("Editor", FontStyle::Regular);
		m_BoldFont = FontManager::Get().GetFont("Editor", FontStyle::Bold);
		m_ItalicFont = FontManager::Get().GetFont("Editor", FontStyle::Italic);
		m_BoldItalicFont = FontManager::Get().GetFont("Editor", FontStyle::BoldItalic);
	}

	EditorView& EditorView::Get()
	{
		static EditorView instance;
		return instance;
	}

	void EditorView::SetBounds(float x, float y, float w, float h)
	{
		m_X = x;
		m_Y = y;
		m_W = w;
		m_H = h;
	}

	void EditorView::SetLineHeight(float lineHeight)
	{
		m_LineHeight = lineHeight;
	}

	void EditorView::HandleScroll(float deltaX, float deltaY)
	{
		m_ScrollX += deltaX;
		m_ScrollY += deltaY;
		ClampScroll();

		if (m_ScrollX < 0)
			m_ScrollX = 0;
		if (m_ScrollY < 0)
			m_ScrollY = 0;
	}

	void EditorView::UpdateCursorBlink(float deltaTime)
	{
		m_CursorBlinkTimer += deltaTime;
		if (m_CursorBlinkTimer >= 0.5f)
		{
			m_CursorBlinkTimer = 0.0f;
			m_CursorVisible = !m_CursorVisible;
		}
	}

	void EditorView::Render(const TextBuffer& buffer, const Cursor& cursor, const Highlight& highlighter, float deltaTime)
	{
		if (m_LineHeight <= 0)
			m_LineHeight = 20.0f;
		m_buffer = buffer;

		if (ThemeManager::IsDarkTheme())
			glColor4f(0.1f, 0.1f, 0.15f, 0.5f);
		else
			glColor4f(0.9f, 0.9f, 0.95f, 0.5f);

		glBegin(GL_POLYGON);
		glVertex2f(m_X, m_Y);
		glVertex2f(m_X + m_W, m_Y);
		glVertex2f(m_X + m_W, m_Y + m_H);
		glVertex2f(m_X, m_Y + m_H);
		glEnd();

		float totalHeight = buffer.GetLineCount() * m_LineHeight;
		int totalLines = buffer.GetLineCount();
		float maxScrollY = std::max(0.0f, totalHeight - m_H + m_LineHeight);
		if (m_ScrollY < 0)
			m_ScrollY = 0;
		if (m_ScrollY > maxScrollY)
			m_ScrollY = maxScrollY;

		if (!m_RegularFont || !m_RegularFont->IsInitialized())
			return;
		if (m_W <= 0 || m_H <= 0)
			return;

		bool needScrollbar = (totalHeight > m_H);
		int scissorW = (int)m_W - (needScrollbar ? (int)m_ScrollbarWidth : 0);
		if (scissorW <= 0)
			scissorW = (int)m_W;
		Application& app = Application::Get();
		int winHeight = app.GetWindow().GetHeight();
		int scissorX = (int)m_X;
		int scissorY = winHeight - (int)(m_Y + m_H);
		int scissorH = (int)m_H;
		glEnable(GL_SCISSOR_TEST);
		glScissor(scissorX, scissorY, scissorW, scissorH);

		UpdateCursorBlink(deltaTime);

		float startY = m_Y - m_ScrollY;

		UpdateLineNumberWidth(buffer);

		if (m_RegularFont && m_RegularFont->IsInitialized())
		{
			float lineNumberX = m_X + 5;
			for (int i = 0; i < totalLines; ++i)
			{
				float y = startY + i * m_LineHeight;
				if (y + m_LineHeight < m_Y || y > m_Y + m_H)
					continue;
				std::string num = std::to_string(i + 1);
				float numWidth = m_RegularFont->GetTextWidth(num);
				float numX = lineNumberX + (m_LineNumberWidth - 10 - numWidth);
				m_RegularFont->DrawText(num, numX, y, 0.5f, 0.5f, 0.6f, 1.0f);
			}
		}

		DrawSelection(buffer, cursor, 0, totalLines, startY);

		CursorPosition pos = cursor.GetPosition();
		FindMatchingBracket(buffer, pos.line, pos.col);
		DrawMatchingBracket(buffer, 0, totalLines, startY);

		if (m_RegularFont && m_RegularFont->IsInitialized())
		{
			float x = m_X + m_LineNumberWidth - m_ScrollX;
			for (int i = 0; i < totalLines; ++i)
			{
				float y = startY + i * m_LineHeight;
				if (y + m_LineHeight < m_Y || y > m_Y + m_H)
					continue;
				const std::string& line = buffer.GetLine(i);
				float curX = x;
				std::vector<HighlightSpan> spans = highlighter.HighlightLine(line);
				int lastPos = 0;
				for (const auto& span : spans)
				{
					if (lastPos < span.start)
					{
						std::string normal = line.substr(lastPos, span.start - lastPos);
						m_RegularFont->DrawText(normal, curX, y, 0.9f, 0.9f, 0.9f, 1.0f);
						curX += m_RegularFont->GetTextWidth(normal);
					}
					std::string highlighted = line.substr(span.start, span.end - span.start);
					TextRenderer* font = m_RegularFont;
					switch (span.color)
					{
					case HighlightColor::Keyword:
						font = m_BoldItalicFont;
						break;
					case HighlightColor::String:
						font = m_BoldFont;
						break;
					case HighlightColor::Comment:
						font = m_ItalicFont;
						break;
					case HighlightColor::Number:
						font = m_RegularFont;
						break;
					case HighlightColor::Preprocessor:
						font = m_BoldFont;
						break;
					default:
						font = m_RegularFont;
						break;
					}

					if (!font)
						font = m_RegularFont;

					glm::vec3 color = highlighter.GetColorForHighlight(span.color);
					font->DrawText(highlighted, curX, y, color.r, color.g, color.b, 1.0f);
					curX += font->GetTextWidth(highlighted);
					lastPos = span.end;
				}

				if (lastPos < (int)line.size())
				{
					std::string rest = line.substr(lastPos);
					m_RegularFont->DrawText(rest, curX, y, 0.9f, 0.9f, 0.9f, 1.0f);
				}
			}
		}

		if (m_CursorVisible)
		{
			CursorPosition pos = cursor.GetPosition();
			if (pos.line >= 0 && pos.line < totalLines)
			{
				float cursorX = m_X + m_LineNumberWidth - m_ScrollX;
				const std::string& line = buffer.GetLine(pos.line);
				std::string prefix = line.substr(0, pos.col);
				cursorX += m_RegularFont->GetTextWidth(prefix);
				float cursorY = m_Y + (pos.line * m_LineHeight) - m_ScrollY;
				DrawCursor(cursor, cursorX, cursorY);
			}
		}

		glDisable(GL_SCISSOR_TEST);
		if (needScrollbar)
			glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_BLEND);
		DrawVerticalScrollbar();
		glPopAttrib();
	}

	void EditorView::DrawLineNumbers(int firstLine, int lastLine, float startY)
	{
		if (!m_RegularFont || !m_RegularFont->IsInitialized())
			return;
		float lineNumberX = m_X + 5;
		float y = startY;
		for (int i = firstLine; i < lastLine; ++i)
		{
			std::string num = std::to_string(i + 1);
			float numWidth = m_RegularFont->GetTextWidth(num);
			float numX = lineNumberX + (m_LineNumberWidth - 10 - numWidth);
			m_RegularFont->DrawText(num, numX, y, 0.5f, 0.5f, 0.6f, 1.0f);
			y += m_LineHeight;
		}
	}

	void EditorView::DrawTextLines(const TextBuffer& buffer, const Highlight& highlighter,
		int firstLine, int lastLine, float startY)
	{
		if (!m_RegularFont || !m_RegularFont->IsInitialized())
			return;
		float x = m_X + m_LineNumberWidth - m_ScrollX;
		float y = startY;
		for (int i = firstLine; i < lastLine; ++i)
		{
			const std::string& line = buffer.GetLine(i);
			float curX = x;

			std::vector<HighlightSpan> spans = highlighter.HighlightLine(line);
			int lastPos = 0;
			for (const auto& span : spans)
			{
				if (lastPos < span.start)
				{
					std::string normal = line.substr(lastPos, span.start - lastPos);
					m_RegularFont->DrawText(normal, curX, y, 0.9f, 0.9f, 0.9f, 1.0f);
					curX += m_RegularFont->GetTextWidth(normal);
				}
				std::string highlighted = line.substr(span.start, span.end - span.start);
				TextRenderer* font = m_RegularFont;
				switch (span.color)
				{
				case HighlightColor::Keyword:
					font = m_BoldItalicFont;
					break;
				case HighlightColor::String:
					font = m_BoldFont;
					break;
				case HighlightColor::Comment:
					font = m_ItalicFont;
					break;
				case HighlightColor::Number:
					font = m_RegularFont;
					break;
				case HighlightColor::Preprocessor:
					font = m_BoldFont;
					break;
				default:
					font = m_RegularFont;
					break;
				}
				if (!font)
					font = m_RegularFont;
				if (!font)
				{
					PS_CORE_ERROR("No valid font available, skip drawing");
				}

				Highlight highlighter;

				glm::vec3 color = highlighter.GetColorForHighlight(span.color);
				font->DrawText(highlighted, curX, y, color.r, color.g, color.b, 1.0f);
				curX += font->GetTextWidth(highlighted);
				lastPos = span.end;
			}
			if (lastPos < (int)line.size())
			{
				std::string rest = line.substr(lastPos);
				m_RegularFont->DrawText(rest, curX, y, 0.9f, 0.9f, 0.9f, 1.0f);
			}
			y += m_LineHeight;
		}
	}

	void EditorView::DrawCursor(const Cursor& cursor, float cursorX, float cursorY)
	{
		if (ThemeManager::IsDarkTheme())
		{
			glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
		}
		else
		{
			glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
		}
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(cursorX, cursorY);
		glVertex2f(cursorX, cursorY + m_LineHeight - 2);
		glEnd();
	}

	CursorPosition EditorView::ScreenToTextPosition(float mx, float my, const TextBuffer& buffer) const
	{
		if (m_LineHeight <= 0)
			return { 0, 0 };
		float localX = mx - (m_X + m_LineNumberWidth);
		float localY = my - m_Y;
		if (localX < 0)
			localX = 0;
		if (localY < 0)
			localY = 0;

		int line = (int)((localY + m_ScrollY) / m_LineHeight);
		if (line < 0)
			line = 0;
		int lineCount = buffer.GetLineCount();
		if (lineCount == 0)
			return { 0, 0 };
		if (line >= lineCount)
			line = lineCount - 1;

		const std::string& lineStr = buffer.GetLine(line);
		int col = 0;
		float accumulated = 0.0f;
		for (char c : lineStr)
		{
			std::string ch(1, c);
			float charWidth = m_RegularFont->GetTextWidth(ch);
			if (accumulated + charWidth / 2 > localX + m_ScrollX)
				break;
			accumulated += charWidth;
			col++;
		}
		return { line, col };
	}

	void EditorView::DrawSelection(const TextBuffer& buffer, const Cursor& cursor,
		int firstLine, int lastLine, float startY)
	{
		if (!cursor.HasSelection())
			return;

		int selStartLine, selStartCol, selEndLine, selEndCol;
		cursor.GetSelectionRange(selStartLine, selStartCol, selEndLine, selEndCol);

		int drawStartLine = std::max(selStartLine, firstLine);
		int drawEndLine = std::min(selEndLine, lastLine - 1);
		if (drawStartLine > drawEndLine)
			return;

		float y = startY + (drawStartLine - firstLine) * m_LineHeight;

		for (int line = drawStartLine; line <= drawEndLine; ++line)
		{
			const std::string& lineStr = buffer.GetLine(line);
			int startCol = (line == selStartLine) ? selStartCol : 0;
			int endCol = (line == selEndLine) ? selEndCol : (int)lineStr.size();
			if (startCol >= endCol)
			{
				y += m_LineHeight;
				continue;
			}

			float xStart = m_X + m_LineNumberWidth - m_ScrollX;
			std::string prefix = lineStr.substr(0, startCol);
			xStart += m_RegularFont->GetTextWidth(prefix);
			float xEnd = xStart;
			std::string selected = lineStr.substr(startCol, endCol - startCol);
			xEnd += m_RegularFont->GetTextWidth(selected);
			float yRect = y;

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.2f, 0.5f, 0.9f, 0.5f);
			glBegin(GL_QUADS);
			glVertex2f(xStart, yRect);
			glVertex2f(xEnd, yRect);
			glVertex2f(xEnd, yRect + m_LineHeight);
			glVertex2f(xStart, yRect + m_LineHeight);
			glEnd();
			y += m_LineHeight;
		}
	}

	void EditorView::FindMatchingBracket(const TextBuffer& buffer, int cursorLine, int cursorCol)
	{
		m_CurrentBracket.valid = false;
		m_MatchPos.valid = false;
		if (cursorLine < 0 || cursorLine >= buffer.GetLineCount())
			return;

		const std::string& line = buffer.GetLine(cursorLine);
		int len = (int)line.size();
		if (cursorCol < 0 || cursorCol >= len)
			return;

		int checkCol = -1;
		char ch = 0;

		if (cursorCol > 0)
		{
			char leftChar = line[cursorCol - 1];
			if (leftChar == '(' || leftChar == '[' || leftChar == '{' ||
				leftChar == ')' || leftChar == ']' || leftChar == '}')
			{
				checkCol = cursorCol - 1;
				ch = leftChar;
			}
		}
		if (checkCol == -1 && cursorCol < len)
		{
			char rightChar = line[cursorCol];
			if (rightChar == '(' || rightChar == '[' || rightChar == '{' ||
				rightChar == ')' || rightChar == ']' || rightChar == '}')
			{
				checkCol = cursorCol;
				ch = rightChar;
			}
		}
		if (checkCol == -1)
			return;

		m_CurrentBracket = { cursorLine, checkCol, true };

		char matchChar = 0;
		bool forward = true;
		switch (ch)
		{
		case '(':
			matchChar = ')';
			forward = true;
			break;
		case '[':
			matchChar = ']';
			forward = true;
			break;
		case '{':
			matchChar = '}';
			forward = true;
			break;
		case ')':
			matchChar = '(';
			forward = false;
			break;
		case ']':
			matchChar = '[';
			forward = false;
			break;
		case '}':
			matchChar = '{';
			forward = false;
			break;
		default:
			return;
		}

		int nestLevel = 1;
		int lineIdx = cursorLine;
		int colIdx = checkCol;
		if (forward)
		{
			colIdx++;
			while (lineIdx < buffer.GetLineCount())
			{
				const std::string& l = buffer.GetLine(lineIdx);
				int start = (lineIdx == cursorLine ? colIdx : 0);
				for (int i = start; i < (int)l.size(); ++i)
				{
					char c = l[i];
					if (c == ch)
						nestLevel++;
					else if (c == matchChar)
					{
						nestLevel--;
						if (nestLevel == 0)
						{
							m_MatchPos = { lineIdx, i, true };
							return;
						}
					}
				}
				lineIdx++;
			}
		}
		else
		{
			colIdx--;
			while (lineIdx >= 0)
			{
				const std::string& l = buffer.GetLine(lineIdx);
				int end = (lineIdx == cursorLine ? colIdx : (int)l.size() - 1);
				for (int i = end; i >= 0; --i)
				{
					char c = l[i];
					if (c == ch)
						nestLevel++;
					else if (c == matchChar)
					{
						nestLevel--;
						if (nestLevel == 0)
						{
							m_MatchPos = { lineIdx, i, true };
							return;
						}
					}
				}
				lineIdx--;
			}
		}
	}

	void EditorView::DrawMatchingBracket(const TextBuffer& buffer, int firstLine, int lastLine, float startY)
	{
		auto drawOne = [&](const BracketMatch& bracket)
			{
				if (!bracket.valid)
					return;
				int line = bracket.line;
				int col = bracket.col;
				if (line < firstLine || line >= lastLine)
					return;
				const std::string& lineStr = buffer.GetLine(line);
				if (col >= (int)lineStr.size())
					return;

				float xStart = m_X + m_LineNumberWidth - m_ScrollX;
				std::string prefix = lineStr.substr(0, col);
				xStart += m_RegularFont->GetTextWidth(prefix);
				float xEnd = xStart + m_RegularFont->GetTextWidth(std::string(1, lineStr[col]));
				float y = startY + (line - firstLine) * m_LineHeight;
				float yRect = y;

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(1.0f, 0.8f, 0.2f, 0.4f);
				glBegin(GL_QUADS);
				glVertex2f(xStart, yRect);
				glVertex2f(xEnd, yRect);
				glVertex2f(xEnd, yRect + m_LineHeight);
				glVertex2f(xStart, yRect + m_LineHeight);
				glEnd();
			};

		drawOne(m_CurrentBracket);
		drawOne(m_MatchPos);
	}

	void EditorView::ClampScroll()
	{
		float totalHeight = m_buffer.GetLineCount() * m_LineHeight;
		float maxScrollY = std::max(0.0f, totalHeight - m_H + m_LineHeight);
		if (m_ScrollY < 0)
			m_ScrollY = 0;
		if (m_ScrollY > maxScrollY)
			m_ScrollY = maxScrollY;

		// Horizontal scrolling is currently unrestricted (expandable in the future)
	}

	void EditorView::EnsureCursorVisible(const Cursor& cursor, const TextBuffer& buffer)
	{
		int line = cursor.GetPosition().line;
		int col = cursor.GetPosition().col;
		float cursorY = line * m_LineHeight;
		float cursorX = col * m_CharWidth;

		if (cursorY < m_ScrollY)
		{
			m_ScrollY = cursorY;
		}
		else if (cursorY + m_LineHeight > m_ScrollY + m_H)
		{
			m_ScrollY = cursorY + m_LineHeight - m_H;
		}

		// Horizontal scroll...

		ClampScroll();
	}

	float EditorView::GetScrollbarThumbHeight() const
	{
		float totalHeight = m_buffer.GetLineCount() * m_LineHeight;
		if (totalHeight <= m_H)
			return m_H;
		float visibleRatio = m_H / totalHeight;
		float thumbHeight = m_H * visibleRatio;
		return std::max(thumbHeight, m_ScrollbarMinThumbHeight);
	}

	float EditorView::GetScrollbarThumbY() const
	{
		float totalHeight = m_buffer.GetLineCount() * m_LineHeight;
		if (totalHeight <= m_H)
			return m_Y;
		float maxScrollY = totalHeight - m_H;
		float scrollRatio = m_ScrollY / maxScrollY;
		float trackHeight = m_H - GetScrollbarThumbHeight();
		return m_Y + scrollRatio * trackHeight;
	}

	void EditorView::DrawVerticalScrollbar()
	{
		float x = m_X + m_W - m_ScrollbarWidth;
		float y = m_Y;
		float w = m_ScrollbarWidth;
		float h = m_H;

		if (ThemeManager::IsDarkTheme())
			glColor4f(0.2f, 0.2f, 0.22f, 1.0f);
		else
			glColor4f(0.8f, 0.8f, 0.82f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();

		float thumbY = GetScrollbarThumbY();
		float thumbH = GetScrollbarThumbHeight();
		glColor4f(0.5f, 0.5f, 0.5f, 0.9f);
		glBegin(GL_QUADS);
		glVertex2f(x, thumbY);
		glVertex2f(x + w, thumbY);
		glVertex2f(x + w, thumbY + thumbH);
		glVertex2f(x, thumbY + thumbH);
		glEnd();
	}

	bool EditorView::OnMouseButton(MouseButtonPressedEvent& e, float mouseX, float mouseY)
	{
		if (e.GetMouseButton() != GLFW_MOUSE_BUTTON_LEFT)
			return false;

		float totalHeight = m_buffer.GetLineCount() * m_LineHeight;
		bool needScrollbar = (totalHeight > m_H);
		if (needScrollbar)
		{
			float scrollbarX = m_X + m_W - m_ScrollbarWidth;
			if (mouseX >= scrollbarX && mouseX <= scrollbarX + m_ScrollbarWidth &&
				mouseY >= m_Y && mouseY <= m_Y + m_H)
			{
				float thumbY = GetScrollbarThumbY();
				float thumbH = GetScrollbarThumbHeight();
				if (mouseY >= thumbY && mouseY <= thumbY + thumbH)
				{
					m_IsDraggingScrollbar = true;
					m_DragStartY = mouseY;
					m_DragStartScrollY = m_ScrollY;
					return true;
				}
				else
				{
					float maxScrollY = totalHeight - m_H + m_LineHeight;
					float clickRatio = (mouseY - m_Y) / m_H;
					float newScrollY = clickRatio * totalHeight - m_H / 2;
					newScrollY = std::max(0.0f, std::min(maxScrollY, newScrollY));
					m_ScrollY = newScrollY;
					return true;
				}
			}
		}
		return false;
	}

	bool EditorView::OnMouseMove(MouseMovedEvent& e, float mouseX, float mouseY)
	{
		if (m_IsDraggingScrollbar)
		{
			float deltaY = mouseY - m_DragStartY;
			float totalHeight = m_buffer.GetLineCount() * m_LineHeight;
			float maxScrollY = totalHeight - m_H;
			float trackHeight = m_H - GetScrollbarThumbHeight();
			float scrollDelta = (deltaY / trackHeight) * maxScrollY;
			m_ScrollY = m_DragStartScrollY + scrollDelta;
			m_ScrollY = std::max(0.0f, std::min(maxScrollY, m_ScrollY));
			return true;
		}
		return false;
	}

	void EditorView::OnMouseRelease()
	{
		m_IsDraggingScrollbar = false;
	}

	void EditorView::UpdateLineNumberWidth(const TextBuffer& buffer)
	{
		int maxLines = buffer.GetLineCount();
		std::string maxLineNumStr = std::to_string(maxLines);

		float maxWidth = m_RegularFont ? m_RegularFont->GetTextWidth(maxLineNumStr) : 0;

		float newWidth = maxWidth + 10.0f;

		m_LineNumberWidth = std::max(newWidth, 30.0f);
	}
}
