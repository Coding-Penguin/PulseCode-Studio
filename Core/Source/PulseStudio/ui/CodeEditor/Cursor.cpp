#include "pspch.h"
#include "Cursor.h"
#include "TextBuffer.h"

namespace PulseStudio {

	Cursor::Cursor()
	{
		m_Position = { 0, 0 };
		m_Anchor = { 0, 0 };
	}

	void Cursor::SetPosition(int line, int col)
	{
		m_Position = { line, col };
	}

	void Cursor::MoveTo(int line, int col)
	{
		SetPosition(line, col);
	}

	void Cursor::Move(int dLine, int dCol, const TextBuffer& buffer, bool extendSelection)
	{
		int newLine = m_Position.line + dLine;
		int newCol = m_Position.col + dCol;

		if (dLine != 0)
		{
			newLine = m_Position.line + dLine;
			newCol = m_Position.col;
		}
		else if (dCol != 0)
		{
			int newPos = m_Position.col + dCol;
			if (newPos < 0)
			{
				if (m_Position.line > 0)
				{
					newLine = m_Position.line - 1;
					newCol = buffer.GetLineLength(newLine);
				}
				else
				{
					newCol = 0;
				}
			}
			else
			{
				int lineLen = buffer.GetLineLength(m_Position.line);
				if (newPos > lineLen)
				{
					if (m_Position.line + 1 < buffer.GetLineCount())
					{
						newLine = m_Position.line + 1;
						newCol = 0;
					}
					else
					{
						newCol = lineLen;
					}
				}
				else
				{
					newCol = newPos;
				}
			}
		}
		else
		{
			return;
		}

		if (newLine < 0) newLine = 0;
		int maxLine = buffer.GetLineCount() - 1;
		if (newLine > maxLine) newLine = maxLine;
		int maxCol = buffer.GetLineLength(newLine);
		if (newCol < 0) newCol = 0;
		if (newCol > maxCol) newCol = maxCol;

		if (extendSelection) 
		{
			if (!m_Selecting)
			{
				m_Selecting = true;
				m_Anchor = m_Position;
			}
			SetPosition(newLine, newCol);
		}
		else
		{
			m_Selecting = false;
			SetPosition(newLine, newCol);
		}
	}

	void Cursor::MoveToLineStart(const TextBuffer& buffer)
	{
		int line = m_Position.line;
		SetPosition(line, 0);
	}

	void Cursor::MoveToLineEnd(const TextBuffer& buffer)
	{
		int line = m_Position.line;
		int maxCol = buffer.GetLineLength(line);
		SetPosition(line, maxCol);
	}

	void Cursor::GetSelectionRange(int& startLine, int& startCol, int& endLine, int& endCol) const
	{
		if (m_Anchor.line < m_Position.line || (m_Anchor.line == m_Position.line && m_Anchor.col < m_Position.col))
		{
			startLine = m_Anchor.line; startCol = m_Anchor.col;
			endLine = m_Position.line; endCol = m_Position.col;
		}
		else 
		{
			startLine = m_Position.line; startCol = m_Position.col;
			endLine = m_Anchor.line; endCol = m_Anchor.col;
		}
	}

}
