#pragma once

#include "TextBuffer.h"

namespace PulseStudio {

	struct CursorPosition
	{
		int line, col;
		bool operator==(const CursorPosition& other) const
		{
			return line == other.line && col == other.col;
		}
	};

	class Cursor
	{
	public:
		Cursor();

		void MoveTo(int line, int col);
		void Move(int dLine, int dCol, const TextBuffer& buffer, bool extendSelection = false);
		void MoveToLineStart(const TextBuffer& buffer);
		void MoveToLineEnd(const TextBuffer& buffer);
		void MoveToTop() { SetPosition(0, 0); }
		void MoveToBottom(const TextBuffer& buffer) { SetPosition(buffer.GetLineCount() - 1, 0); }

		CursorPosition GetPosition() const { return m_Position; }
		void SetPosition(int line, int col);

		CursorPosition GetSelectionAnchor() const { return m_Anchor; }
		bool HasSelection() const { return m_Selecting && !(m_Anchor == m_Position); }
		void StartSelection() { m_Selecting = true; m_Anchor = m_Position; }
		void EndSelection() { m_Selecting = false; }
		bool IsSelecting() const { return m_Selecting; }
		void SetSelecting(bool selecting) { m_Selecting = selecting; if (selecting) m_Anchor = m_Position; }

		void GetSelectionRange(int& startLine, int& startCol, int& endLine, int& endCol) const;

		void SetSelectionAnchor(const CursorPosition& anchor) { m_Anchor = anchor; }
	private:
		bool m_Selecting = false;

		CursorPosition m_Position;
		CursorPosition m_Anchor;
	};

}
