#include "pspch.h"
#include "TextBuffer.h"
#include <sstream>
#include <algorithm>

namespace PulseStudio {

	TextBuffer::TextBuffer()
	{
		m_Lines.emplace_back();
	}

	void TextBuffer::LoadFromFile(const std::string& filePath)
	{
		m_FilePath = filePath;
		m_Lines.clear();
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			m_Lines.emplace_back();
			return;
		}
		std::string line;
		while (std::getline(file, line))
		{
			m_Lines.push_back(line);
		}
		if (m_Lines.empty())
		{
			m_Lines.emplace_back();
		}
	}

	std::string TextBuffer::GetString() const
	{
		std::string result;
		for (size_t i = 0; i < m_Lines.size(); ++i)
		{
			result += m_Lines[i];
			if (i != m_Lines.size() - 1)
			{
				result += '\n';
			}
		}
		return result;
	}

	void TextBuffer::InsertChar(int line, int col, char c)
	{
		if (line < 0 || line >= (int)m_Lines.size()) return;
		if (col < 0) col = 0;
		std::string& target = m_Lines[line];
		if (col > (int)target.size()) col = (int)target.size();
		target.insert(col, 1, c);
	}

	void TextBuffer::DeleteChar(int line, int col)
	{
		if (line < 0 || line >= (int)m_Lines.size()) return;
		std::string& target = m_Lines[line];
		if (col < 0 || col >= (int)target.size()) return;
		target.erase(col, 1);
	}

	void TextBuffer::InsertNewline(int line, int col)
	{
		if (line < 0 || line >= (int)m_Lines.size()) return;
		if (col < 0) col = 0;
		std::string& current = m_Lines[line];
		if (col > (int)current.size()) col = (int)current.size();

		std::string left = current.substr(0, col);
		std::string right = current.substr(col);
		m_Lines[line] = left;
		m_Lines.insert(m_Lines.begin() + line + 1, right);
	}

	void TextBuffer::DeleteRange(int startLine, int startCol, int endLine, int endCol)
	{
		if (startLine < 0 || endLine < 0) return;
		if (startLine > endLine) return;
		if (startLine == endLine && startCol > endCol) return;
		if (startLine >= (int)m_Lines.size()) startLine = (int)m_Lines.size() - 1;
		if (endLine >= (int)m_Lines.size()) endLine = (int)m_Lines.size() - 1;
		if (startCol < 0) startCol = 0;
		if (endCol < 0) endCol = 0;

		const std::string& firstLine = m_Lines[startLine];
		const std::string& lastLine = m_Lines[endLine];
		if (startCol > (int)firstLine.size()) startCol = (int)firstLine.size();
		if (endCol > (int)lastLine.size()) endCol = (int)lastLine.size();

		if (startLine == endLine)
		{
			std::string& line = m_Lines[startLine];
			line.erase(startCol, endCol - startCol);
		}
		else
		{
			std::string newLine = m_Lines[startLine].substr(0, startCol) +
				m_Lines[endLine].substr(endCol);
			m_Lines[startLine] = newLine;
			m_Lines.erase(m_Lines.begin() + startLine + 1, m_Lines.begin() + endLine + 1);
		}
	}

	void TextBuffer::InsertLine(int line, const std::string& content)
	{
		if (line < 0 || line >(int)m_Lines.size()) return;
		m_Lines.insert(m_Lines.begin() + line, content);
	}

	void TextBuffer::DeleteLine(int line)
	{
		if (line >= 0 && line < (int)m_Lines.size())
		{
			m_Lines.erase(m_Lines.begin() + line);
		}
	}

}
