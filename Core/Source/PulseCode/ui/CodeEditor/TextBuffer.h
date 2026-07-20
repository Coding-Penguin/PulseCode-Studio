#pragma once
#include "pspch.h"

namespace PulseCode {

	class TextBuffer
	{
	public:
		TextBuffer();
		void LoadFromFile(const std::string& path);
		std::string GetString() const;

		void InsertChar(int line, int col, char c);
		void DeleteChar(int line, int col);
		void InsertNewline(int line, int col);
		void DeleteRange(int startLine, int startCol, int endLine, int endCol);

		int GetLineCount() const { return (int)m_Lines.size(); }
		const std::string& GetLine(int line) const
		{
			static const std::string empty;
			if (line < 0 || line >= (int)m_Lines.size())
				return empty; return m_Lines[line];
		}
		void SetLine(int line, const std::string& newText) { m_Lines[line] = newText; }

		void InsertLine(int line, const std::string& content);

		void DeleteLine(int line);

		void LoadFromString(const std::string& text);

		int GetLineLength(int line) const { return (int)m_Lines[line].size(); }
		std::string GetFilePath() const { return m_FilePath; }
	private:
		std::vector<std::string> m_Lines;
		std::string m_FilePath;
	};

}
