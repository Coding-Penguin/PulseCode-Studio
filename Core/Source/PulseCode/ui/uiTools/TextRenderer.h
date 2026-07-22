#pragma once
#include <string>
#include <unordered_map>
#include <stb_truetype.h>

namespace PulseCode {

	class TextRenderer
	{
	public:
		TextRenderer() = default;
		~TextRenderer();

		static TextRenderer& Get();

		bool LoadFont(const std::string& fontPath, float fontSize);
		void DrawText(const std::string& text, float x, float y, float r, float g, float b, float a);
		float GetTextWidth(const std::string& text) const;
		float GetTextHeight() const;

		static float GetFontSize();

		void Unload();

		bool IsInitialized() const { return m_Initialized; }
	private:
		struct CharInfo
		{
			float advance;
			float width, height;
			float x0, y0;
			float x1, y1;
			float xoff, yoff;
		};

		bool m_Initialized = false;
		unsigned int m_TextureID = 0;
		int m_AtlasWidth = 0, m_AtlasHeight = 0;
		std::unordered_map<char, CharInfo> m_Chars;
		static float m_FontSize;
	};

}