#pragma once

#include "pspch.h"
#include "PulseCode/Events/Event.h"
#include "PhotoRenderer.h"
#include "uiButton.h"
#include "Search.h"

namespace PulseCode
{

	class uiTitleBar
	{
	public:
		uiTitleBar();
		~uiTitleBar();
		void OnAttach();
		void OnDetach();
		void OnUpdate(float deltaTime);
		bool OnEvent(Event& event);

		void Draw();
		void OnWindowResize(int width, int height);

		void DrawMinimizeButton(float x, float y, float w, float h, bool hovered);
		void DrawMaximizeButton(float x, float y, float w, float h, bool hovered, bool isMaximized);
		void DrawCloseButton(float x, float y, float w, float h, bool hovered);
	private:
		float m_AccumulatedDragX = 0.0f;
		float m_AccumulatedDragY = 0.0f;

		std::unique_ptr<PhotoRenderer> m_Logo;
		std::vector<std::unique_ptr<uiButton>> m_Buttons;

		void UpdateWindowButtonsPosition(int windowWidth);

		struct ButtonRect
		{ 
			float x, y, w, h; 
		};
		ButtonRect m_MinimizeRect, m_MaximizeRect, m_CloseRect;
		bool m_MinimizeHovered = false;
		bool m_MaximizeHovered = false;
		bool m_CloseHovered = false;

		bool m_DraggingMainWindow = false;
		float m_DragStartX = 0.0f, m_DragStartY = 0.0f;
		int m_WindowStartX = 0, m_WindowStartY = 0;
	};

}
