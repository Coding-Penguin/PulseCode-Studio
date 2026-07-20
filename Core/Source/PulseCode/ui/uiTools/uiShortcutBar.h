#pragma once
#include "pspch.h"
#include "uiButton.h"
#include <vector>
#include <functional>

namespace PulseCode {

    struct ShortcutItem
    {
        std::string id;
        std::string text;
        std::string tooltip;
        std::function<void()> callback;
        bool enabled = true;
    };

    struct ShortcutGroup
    {
        std::vector<ShortcutItem> items;
        bool hasSeparatorAfter = false;
        std::vector<uiButton*> buttons;
    };

    class uiShortcutBar
    {
    public:
        uiShortcutBar();
        ~uiShortcutBar();

        void OnAttach();
        void OnDetach();
        void OnUpdate(float deltaTime);
        bool OnEvent(Event& event);
        void Draw();

        void AddGroup(const std::vector<ShortcutItem>& items, bool separatorAfter = false);
        void Clear();
    private:
        std::vector<std::unique_ptr<uiButton>> m_Buttons;
        std::vector<ShortcutGroup> m_Groups;
        float m_BarHeight = 50.0f;
        float m_ButtonWidth = 40.0f;
        float m_ButtonHeight = 25.0f;
        float m_Padding = 5.0f;
        float m_SeparatorMargin = 5.0f;
        float m_OffsetY = 45.0f;

        std::vector<float> m_GroupEndX;
        void RebuildButtons();
    };

}