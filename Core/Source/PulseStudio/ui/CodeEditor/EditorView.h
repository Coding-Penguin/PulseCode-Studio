#pragma once
#include "TextBuffer.h"
#include "Cursor.h"
#include "HighLight.h"
#include <GLFW/glfw3.h>
#include "../uiTools/TextRenderer.h"

namespace PulseStudio {

	class TextRenderer;

    class EditorView
    {
    public:
        EditorView();
        void SetBounds(float x, float y, float w, float h);
        void Render(const TextBuffer& buffer, const Cursor& cursor,
            const Highlight& highlighter,
            float deltaTime);

        void HandleScroll(float deltaX, float deltaY);
        void UpdateCursorBlink(float deltaTime);
        bool IsCursorVisible() const { return m_CursorVisible; }

        CursorPosition ScreenToTextPosition(float mx, float my, const TextBuffer& buffer) const;

        void DrawSelection(const TextBuffer& buffer, const Cursor& cursor, int firstLine, int lastLine, float startY);

        void SetLineHeight(float lineHeight);

        void EnsureCursorVisible(const Cursor& cursor, const TextBuffer& buffer);

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }
		float GetWidth() const { return m_W; }
		float GetHeight() const { return m_H; }
        float GetFontSize() const { return m_FontSize; }
    private:
        std::string m_FontName = "CascadiaCode";
        float m_FontSize = 24.0f;

        float m_X, m_Y, m_W, m_H;
        float m_ScrollX = 0.0f, m_ScrollY = 0.0f;
        float m_LineHeight = m_FontSize;
        float m_CharWidth = 12.0f;
        float m_LineNumberWidth = 50.0f;

        TextRenderer* m_RegularFont = nullptr;
        TextRenderer* m_BoldFont = nullptr;
        TextRenderer* m_ItalicFont = nullptr;
		TextRenderer* m_BoldItalicFont = nullptr;

        float m_CursorBlinkTimer = 0.0f;
        bool m_CursorVisible = true;

        std::string m_FontPathRegular;
        std::string m_FontPathBold;
        std::string m_FontPathItalic;
        std::string m_FontPathBoldItalic;

        struct BracketMatch
        {
            int line;
            int col;
            bool valid = false;
        };
        BracketMatch m_CurrentBracket;
        BracketMatch m_MatchPos;

        TextBuffer m_buffer;

        void FindMatchingBracket(const TextBuffer& buffer, int cursorLine, int cursorCol);
        void DrawMatchingBracket(const TextBuffer& buffer, int firstLine, int lastLine, float startY);

        void DrawLineNumbers(int firstLine, int lastLine, float startY);
        void DrawTextLines(const TextBuffer& buffer, const Highlight& highlighter,
            int firstLine, int lastLine, float startY);
        void DrawCursor(const Cursor& cursor, float cursorX, float cursorY);
        void ClampScroll();
    };

}