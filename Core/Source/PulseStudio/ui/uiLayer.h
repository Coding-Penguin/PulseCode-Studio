#pragma once
#include "PulseStudio/Layer.h"
#include <vector>

namespace PulseStudio {

    class uiWindow;
    class uiMenuBar;
	class uiButton;
	class MouseCircle;

    class uiLayer : public Layer
    {
    public:
        uiLayer();
        virtual ~uiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual bool OnEvent(Event& event) override;

        void AddWindow(uiWindow* window);
    private:
        std::vector<uiWindow*> m_Windows;
        uiMenuBar* menuBar = nullptr;
    };

}
