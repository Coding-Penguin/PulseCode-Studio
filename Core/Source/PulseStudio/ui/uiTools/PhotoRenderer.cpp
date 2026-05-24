#include "pspch.h"
#include "PhotoRenderer.h"

// 确保 STB_IMAGE_IMPLEMENTATION 只定义一次
// 如果你的项目中已经有地方定义了，可以注释掉下面这行
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
#include "PulseStudio/Log.h"

namespace PulseStudio {

    PhotoRenderer::PhotoRenderer()
    {
    }

    PhotoRenderer::~PhotoRenderer() 
    {
        Unload();
    }

    bool PhotoRenderer::LoadFromFile(const std::string& filepath)
    {
        Unload();

        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_Channels, 4);
        if (!data)
        {
            PS_CORE_ERROR("Failed to load image: {}", filepath);
            return false;
        }

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        PS_CORE_INFO("Loaded image: {} ({} x {})", filepath, m_Width, m_Height);
        return true;
    }

    void PhotoRenderer::Unload()
    {
        if (m_TextureID) 
        {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
        }
        m_Width = m_Height = m_Channels = 0;
    }

    void PhotoRenderer::Draw(float x, float y, float width, float height) const
    {
        if (!IsLoaded()) return;

        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopAttrib();
    }

}
