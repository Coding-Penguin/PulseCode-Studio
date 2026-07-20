#pragma once
#include "PulseCode/Layer.h"

namespace PulseCode {

    class uiStatusBar : public Layer
    {
    public:
        uiStatusBar();
        virtual ~uiStatusBar();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual bool OnEvent(Event& event) override;

        void SetStatusText(const std::string& text);
        void SetRightText(const std::string& text);

        void SetProgress(float progress);
    private:
        void DrawBackground();
        void DrawText();
        void DrawProgress();
        void DrawChannel();

        std::string m_StatusText = "Ready";
        std::string m_RightText = "";

        float m_Progress = -1.0f;
        float m_Height = 35.0f;
    };

}
