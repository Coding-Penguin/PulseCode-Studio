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

        void SetFontSize(float fontSize);
        void SetLineHeight(float lineHeight);

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }
		float GetWidth() const { return m_W; }
		float GetHeight() const { return m_H; }
    private:
        float m_X, m_Y, m_W, m_H;
        float m_ScrollX = 0.0f, m_ScrollY = 0.0f;
        float m_LineHeight = 20.0f;
        float m_CharWidth = 12.0f;
        float m_LineNumberWidth = 50.0f;

        std::string m_FontName = "CascadiaMono";
        float m_FontSize = 20.0f;

        TextRenderer* m_RegularFont = nullptr;
        TextRenderer* m_BoldFont = nullptr;
        TextRenderer* m_ItalicFont = nullptr;
		TextRenderer* m_BoldItalicFont = nullptr;

        float m_CursorBlinkTimer = 0.0f;
        bool m_CursorVisible = true;

        void DrawLineNumbers(int firstLine, int lastLine, float startY);
        void DrawTextLines(const TextBuffer& buffer, const Highlight& highlighter,
            int firstLine, int lastLine, float startY);
        void DrawCursor(const Cursor& cursor, float cursorX, float cursorY);
    };

}