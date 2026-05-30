#include "pspch.h"
#include "FontManager.h"

namespace PulseStudio {

    FontManager& FontManager::Get()
    {
        static FontManager instance;
        return instance;
    }

    bool FontManager::LoadFont(const std::string& name, const std::string& filepath, float size, FontStyle style)
    {
        FontKey key{ name, style };
        if (m_Fonts.find(key) != m_Fonts.end()) return true;
        auto font = std::make_unique<TextRenderer>();
        if (!font->LoadFont(filepath, size))
        {
            PS_CORE_ERROR("Failed to load font: {} at size {}", filepath, size);
            return false;
        }
        m_Fonts[key] = std::move(font);
        return true;
    }

    TextRenderer* FontManager::GetFont(const std::string& name, FontStyle style)
    {
        FontKey key{ name, style };
        auto it = m_Fonts.find(key);
        if (it != m_Fonts.end()) return it->second.get();
        return nullptr;
    }

    void FontManager::UnloadAll()
    {
        m_Fonts.clear();
    }

}