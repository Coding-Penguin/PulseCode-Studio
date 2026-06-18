#pragma once

namespace PulseStudio {

	class TextBuffer
	{
    public:
        TextBuffer();
        void LoadFromString(const std::string& text);
        std::string GetString() const;

        void InsertChar(int line, int col, char c);
        void DeleteChar(int line, int col);
        void InsertNewline(int line, int col);
        void DeleteRange(int startLine, int startCol, int endLine, int endCol);

        int GetLineCount() const { return (int)m_Lines.size(); }
        const std::string& GetLine(int line) const { return m_Lines[line]; }
        void SetLine(int line, const std::string& newText) { m_Lines[line] = newText; }

        void InsertLine(int line, const std::string& content);

        void DeleteLine(int line);

        int GetLineLength(int line) const { return (int)m_Lines[line].size(); }
    private:
        std::vector<std::string> m_Lines;
    };

}
