#include "pspch.h"
#include "TextRenderer.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PulseStudio/Log.h"
#include <fstream>

namespace PulseStudio {

	float TextRenderer::m_FontSize = 0.0f;

	TextRenderer& TextRenderer::Get()
	{
		static TextRenderer instance;
		return instance;
	}

	TextRenderer::~TextRenderer()
	{
		if (m_TextureID) glDeleteTextures(1, &m_TextureID);
	}

	bool TextRenderer::LoadFont(const std::string& fontPath, float fontSize)
	{
		if (m_Initialized) Unload();
		std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
		if (!file)
		{
			PS_CORE_ERROR("Failed to open font file: {}", fontPath);
			return false;
		}
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<unsigned char> buffer(size);
		if (!file.read((char*)buffer.data(), size))
		{
			PS_CORE_ERROR("Failed to read font file: {}", fontPath);
			return false;
		}
		file.close();

		stbtt_fontinfo info;
		if (!stbtt_InitFont(&info, buffer.data(), 0))
		{
			PS_CORE_ERROR("Failed to init font: {}", fontPath);
			return false;
		}

		m_FontSize = fontSize;
		float scale = stbtt_ScaleForPixelHeight(&info, fontSize);

		const int atlasW = 512, atlasH = 512;
		unsigned char* texData = new unsigned char[atlasW * atlasH];
		memset(texData, 0, atlasW * atlasH);

		stbtt_bakedchar bakedChars[95];
		int result = stbtt_BakeFontBitmap(buffer.data(), 0, fontSize, texData, atlasW, atlasH, 32, 95, bakedChars);
		if (result == 0)
		{
			PS_CORE_ERROR("Failed to bake font bitmap");
			delete[] texData;
			return false;
		}

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, atlasW, atlasH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, 0);
		delete[] texData;

		for (int i = 0; i < 95; ++i)
		{
			char c = (char)(32 + i);
			auto& bc = bakedChars[i];
			CharInfo info;
			info.x0 = bc.x0 / (float)atlasW;
			info.y0 = bc.y0 / (float)atlasH;
			info.x1 = bc.x1 / (float)atlasW;
			info.y1 = bc.y1 / (float)atlasH;
			info.width = bc.x1 - bc.x0;
			info.height = bc.y1 - bc.y0;
			info.advance = bc.xadvance;
			info.xoff = bc.xoff;
			info.yoff = bc.yoff + 17.0f;
			m_Chars[c] = info;
		}

		m_Initialized = true;
		PS_CORE_TRACE("Font loaded: {}", fontPath);

		return true;
	}

	void TextRenderer::DrawText(const std::string& text, float x, float y, float r, float g, float b, float a)
	{
		if (this == nullptr) return;
		if (!m_Initialized)
		{
			PS_CORE_ERROR("TextRenderer not initialized or no atlas");
			return;
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glColor4f(r, g, b, a);
		glBegin(GL_QUADS);
		float curX = x;
		float curY = y;
		for (char c : text)
		{
			auto it = m_Chars.find(c);
			if (it == m_Chars.end()) continue;
			const CharInfo& ci = it->second;
			float x0 = curX;
			float y0 = curY;
			float x1 = curX + ci.width;
			float y1 = curY + ci.height;
			float xpos = curX + ci.xoff;
			float ypos = curY + ci.yoff;
			glTexCoord2f(ci.x0, ci.y0); glVertex2f(xpos, ypos);
			glTexCoord2f(ci.x1, ci.y0); glVertex2f(xpos + ci.width, ypos);
			glTexCoord2f(ci.x1, ci.y1); glVertex2f(xpos + ci.width, ypos + ci.height);
			glTexCoord2f(ci.x0, ci.y1); glVertex2f(xpos, ypos + ci.height);
			curX += ci.advance;
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	float TextRenderer::GetTextWidth(const std::string& text) const
	{
		if (this == nullptr) return 0.0f;
		if (!m_Initialized)
		{
			PS_CORE_ERROR("TextRenderer not initialized or no atlas");
			return 0.0f;
		}
		float width = 0.0f;
		for (char c : text)
		{
			auto it = m_Chars.find(c);
			if (it != m_Chars.end())
				width += it->second.advance;
			else
				width += m_FontSize * 0.5f;
		}
		return width;
	}

	float TextRenderer::GetTextHeight() const
	{
		return m_FontSize;
	}

	void TextRenderer::Unload()
	{
		if (m_TextureID)
		{
			glDeleteTextures(1, &m_TextureID);
			m_TextureID = 0;
		}
	}

	float TextRenderer::GetFontSize()
	{
		return m_FontSize;
	}

}
