#pragma once
#include <string>
#include <memory>

namespace PulseStudio {

    class PhotoRenderer
    {
    public:
        PhotoRenderer();
        ~PhotoRenderer();

        bool LoadFromFile(const std::string& filepath);

        void Unload();

        void Draw(float x, float y, float width, float height) const;

        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }

        bool IsLoaded() const { return m_TextureID != 0; }
    private:
        unsigned int m_TextureID = 0;
        int m_Width = 0, m_Height = 0, m_Channels = 0;
    };

}