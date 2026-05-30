#include "pspch.h"
#include "EditorView.h"
#include "../uiTools/TextRenderer.h"
#include "PulseStudio/Application.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cmath>

#include "PulseStudio/FontManager.h"
#include "HighLight.h"

namespace PulseStudio {

	EditorView::EditorView()
	{
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/Pulse-Studio/Core/Resources/Fonts/CascadiaMono.ttf", m_FontSize, FontStyle::Regular);
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/Pulse-Studio/Core/Resources/Fonts/CascadiaMono-Bold.ttf", m_FontSize, FontStyle::Bold);
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/Pulse-Studio/Core/Resources/Fonts/CascadiaMono-Italic.ttf", m_FontSize, FontStyle::Italic);
		FontManager::Get().LoadFont("Editor", "H:/Projects/CppProject/Pulse-Studio/Core/Resources/Fonts/CascadiaMono-BoldItalic.ttf", m_FontSize, FontStyle::BoldItalic);

		m_RegularFont = FontManager::Get().GetFont("Editor", FontStyle::Regular);
		m_BoldFont = FontManager::Get().GetFont("Editor", FontStyle::Bold);
		m_ItalicFont = FontManager::Get().GetFont("Editor", FontStyle::Italic);
		m_BoldItalicFont = FontManager::Get().GetFont("Editor", FontStyle::BoldItalic);
	}

	void EditorView::SetBounds(float x, float y, float w, float h)
	{
		m_X = x; m_Y = y; m_W = w; m_H = h;
	}

	void EditorView::SetFontSize(float fontSize)
	{
		m_LineHeight = fontSize * 1.2f;
		m_CharWidth = fontSize * 0.6f;
	}

	void EditorView::SetLineHeight(float lineHeight)
	{
		m_LineHeight = lineHeight;
	}

	void EditorView::HandleScroll(float deltaX, float deltaY)
	{
		m_ScrollX += deltaX;
		m_ScrollY += deltaY;

		if (m_ScrollX < 0) m_ScrollX = 0;
		if (m_ScrollY < 0) m_ScrollY = 0;
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

	void EditorView::Render(const TextBuffer& buffer, const Cursor& cursor,
		const Highlight& highlighter,
		float deltaTime)
	{
		if (m_W <= 0 || m_H <= 0) return;

		UpdateCursorBlink(deltaTime);

		int firstLine = (int)(m_ScrollY / m_LineHeight);
		int lastLine = firstLine + (int)(m_H / m_LineHeight) + 2;
		int totalLines = buffer.GetLineCount();
		if (firstLine < 0) firstLine = 0;
		if (lastLine > totalLines) lastLine = totalLines;

		float startY = m_Y - m_ScrollY;

		DrawLineNumbers(firstLine, lastLine, startY);
		DrawSelection(buffer, cursor, firstLine, lastLine, startY);
		DrawTextLines(buffer, highlighter, firstLine, lastLine, startY);

		if (m_CursorVisible)
		{
			CursorPosition pos = cursor.GetPosition();
			if (pos.line >= firstLine && pos.line < lastLine)
			{
				float cursorX = m_X + m_LineNumberWidth - m_ScrollX;
				const std::string& line = buffer.GetLine(pos.line);
				std::string prefix = line.substr(0, pos.col);
				cursorX += m_RegularFont->GetTextWidth(prefix);
				float cursorY = m_Y + (pos.line - firstLine) * m_LineHeight - m_ScrollY;
				DrawCursor(cursor, cursorX, cursorY);
			}
		}
	}

	void EditorView::DrawLineNumbers(int firstLine, int lastLine, float startY)
	{
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
				case HighlightColor::Keyword:   font = m_BoldItalicFont; break;
				case HighlightColor::String:    font = m_BoldFont; break;
				case HighlightColor::Comment:   font = m_ItalicFont; break;
				case HighlightColor::Number:    font = m_RegularFont; break;
				case HighlightColor::Preprocessor: font = m_BoldFont; break;
				case HighlightColor::Macro:     font = m_BoldFont; break;
				default:                        font = m_RegularFont; break;
				}
				if (!font) font = m_RegularFont;

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
		glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(cursorX, cursorY);
		glVertex2f(cursorX, cursorY + m_LineHeight);
		glEnd();
	}

	CursorPosition EditorView::ScreenToTextPosition(float mx, float my, const TextBuffer& buffer) const
	{
		float localX = mx - (m_X + m_LineNumberWidth);
		float localY = my - m_Y;
		if (localX < 0) localX = 0;
		if (localY < 0) localY = 0;

		int line = (int)((localY + m_ScrollY) / m_LineHeight);
		if (line < 0) line = 0;
		if (line >= buffer.GetLineCount()) line = buffer.GetLineCount() - 1;

		const std::string& lineStr = buffer.GetLine(line);
		int col = 0;
		float accumulated = 0.0f;
		for (char c : lineStr)
		{
			std::string ch(1, c);
			float charWidth = m_RegularFont->GetTextWidth(ch);
			if (accumulated + charWidth / 2 > localX + m_ScrollX) break;
			accumulated += charWidth;
			col++;
		}
		return { line, col };
	}

	void EditorView::DrawSelection(const TextBuffer& buffer, const Cursor& cursor,
		int firstLine, int lastLine, float startY)
	{
		if (!cursor.HasSelection()) return;
		int selStartLine, selStartCol, selEndLine, selEndCol;
		cursor.GetSelectionRange(selStartLine, selStartCol, selEndLine, selEndCol);

		int drawStartLine = std::max(selStartLine, firstLine);
		int drawEndLine = std::min(selEndLine, lastLine - 1);
		if (drawStartLine > drawEndLine) return;

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

}
