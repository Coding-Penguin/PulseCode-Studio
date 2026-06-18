#include "pspch.h"
#include "uiStatusBar.h"
#include <glad/glad.h>
#include "PulseStudio/Application.h"
#include "PulseStudio/Log.h"
#include "TextRenderer.h"
#include "PulseStudio/Theme.h"
#include "PulseStudio/Channel.h"

namespace PulseStudio {

    uiStatusBar::uiStatusBar()
        : Layer("StatusBar")
    {
    }

    uiStatusBar::~uiStatusBar()
    {
    }

    void uiStatusBar::OnAttach()
    {
        PS_CORE_INFO("uiStatusBar attached");
    }

    void uiStatusBar::OnDetach()
    {
        PS_CORE_INFO("uiStatusBar detached");
    }

    void uiStatusBar::OnUpdate(float deltaTime)
    {
        Application& app = Application::Get();
        int width = (int)app.GetWindow().GetWidth();
        int height = (int)app.GetWindow().GetHeight();
        if (width == 0 || height == 0) return;

        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
        glPushMatrix();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        DrawBackground();
        DrawProgress();
        DrawChannel();
        DrawText();

        glPopMatrix();
        glPopAttrib();
    }

    bool uiStatusBar::OnEvent(Event& event)
    {
        return false;
    }

    void uiStatusBar::SetStatusText(const std::string& text)
    {
        m_StatusText = text;
    }

    void uiStatusBar::SetRightText(const std::string& text)
    {
        m_RightText = text;
    }

    void uiStatusBar::SetProgress(float progress)
    {
        m_Progress = std::clamp(progress, -1.0f, 1.0f);
    }

    void uiStatusBar::DrawBackground()
    {
        Application& app = Application::Get();
        int width = (int)app.GetWindow().GetWidth();
        int height = (int)app.GetWindow().GetHeight();
        float y = height - m_Height;

        if (ThemeManager::GetCurrentTheme() == Theme::Dark)
        {
            glColor4f(0.1f, 0.1f, 0.15f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Light)
        {
            glColor4f(0.9f, 0.9f, 0.9f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Cool_Breeze)
        {
            glColor4f(0.75f, 0.85f, 0.9f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Cool_Slate)
        {
            glColor4f(0.150f, 0.256f, 0.355f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Icy_Mint)
        {
            glColor4f(0.75f, 0.85f, 0.8f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Moonlight)
        {
            glColor4f(0.15f, 0.2f, 0.32f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Hacker)
        {
            glColor4f(0.072f, 0.075f, 0.09f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Sand)
        {
            glColor4f(0.9f, 0.85f, 0.65f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Ice)
        {
            glColor4f(0.8f, 0.8f, 0.85f, 1);
        }
        else if (ThemeManager::GetCurrentTheme() == Theme::Grape)
        {
            glColor4f(0.25f, 0.05f, 0.35f, 1);
        }

        glBegin(GL_QUADS);
        glVertex2f(0, y);
        glVertex2f(width, y);
        glVertex2f(width, y + m_Height);
        glVertex2f(0, y + m_Height);
        glEnd();
    }

    void uiStatusBar::DrawChannel()
    {
        Channel channel = ChannelManager::GetChannel();
        if (channel == Channel::Preview)
        {
            m_RightText = "Preview";
        }
        else
        {
            m_RightText = "Current";
        }
        DrawText();
    }

    void uiStatusBar::DrawText()
    {
        Application& app = Application::Get();
        int width = (int)app.GetWindow().GetWidth();
        int height = (int)app.GetWindow().GetHeight();
        float y = height - m_Height;
        float textY = y + (m_Height - TextRenderer::Get().GetTextHeight()) / 2;

        float r, g, b;
        if (ThemeManager::IsDarkTheme())
        {
            r = g = b = 0.85f;
        }
        else
        {
            r = g = b = 0.15f;
        }

        float leftX = 10.0f;
        TextRenderer::Get().DrawText(m_StatusText, leftX, textY, r, g, b, 1.0f);

        if (!m_RightText.empty())
        {
            float rightTextWidth = TextRenderer::Get().GetTextWidth(m_RightText);
            float rightX = width - rightTextWidth - 10.0f;
            TextRenderer::Get().DrawText(m_RightText, rightX, textY, r, g, b, 1.0f);
        }
    }

    void uiStatusBar::DrawProgress()
    {
        if (m_Progress < 0.0f) return;

        Application& app = Application::Get();
        int width = (int)app.GetWindow().GetWidth();
        int height = (int)app.GetWindow().GetHeight();
        float y = height - m_Height;

        float rightTextWidth = m_RightText.empty() ? 0 : TextRenderer::Get().GetTextWidth(m_RightText);
        float rightX = width - rightTextWidth - 20.0f;
        float leftX = rightX - 150.f;
        float barWidth = rightX - leftX;
        if ((barWidth < 0.0f) || (leftX < (TextRenderer::Get().GetTextWidth(m_StatusText) + 5.0f))) return;

        float barHeight = 15.0f;
        float barY = y + (m_Height - barHeight) / 2;

        if (ThemeManager::IsDarkTheme())
        {
            glColor4f(0.3f, 0.3f, 0.35f, 0.7f);
        }
        else
        {
            glColor4f(0.7f, 0.7f, 0.75f, 0.7f);
        }
        glBegin(GL_QUADS);
        glVertex2f(leftX, barY);
        glVertex2f(leftX + barWidth, barY);
        glVertex2f(leftX + barWidth, barY + barHeight);
        glVertex2f(leftX, barY + barHeight);
        glEnd();

        float fillWidth = barWidth * m_Progress;
        glColor4f(0.2f, 0.5f, 0.95f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(leftX, barY);
        glVertex2f(leftX + fillWidth, barY);
        glVertex2f(leftX + fillWidth, barY + barHeight);
        glVertex2f(leftX, barY + barHeight);
        glEnd();
    }

}
