#pragma once
#include "pspch.h"
#include "TextRenderer.h"
#include "PulseStudio/Layer.h"
#include "uiButton.h"

namespace PulseStudio {

	enum class ResizeEdge
	{
		None,
		Left, Right, Top, Bottom,
		TopLeft, TopRight, BottomLeft, BottomRight
	};

	class uiWindow : public Layer
	{
	public:
		uiWindow(std::string name);
		virtual ~uiWindow();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual bool OnEvent(Event& event) override;

		bool IsVisible() const { return m_IsVisible; }

		void SetSize(float x, float y, float width, float height);
		void SetStyle(bool isDark);

		void AddButton(uiButton* button);

		void DrawContent();
		std::string GetTitle() const { return m_name; }

		void SetTitle(const std::string& title) { m_name = title; }

		int GetX() const { return (int)m_RectX; }
		int GetY() const { return (int)m_RectY; }
		int GetWidth() const { return (int)m_RectWidth; }
		int GetHeight() const { return (int)m_RectHeight; }

		void SetDraggable(bool draggable) { m_IsDraggable = draggable; }
		void SetResizable(bool resizable) { m_IsResizable = resizable; }
	private:
		bool m_IsDraggable = true;
		bool m_IsResizable = true;

		std::string m_name = "uiWindow";
		float m_RectX = 100.0f;
		float m_RectY = 150.0f;
		float m_RectWidth = 500.0f;
		float m_RectHeight = 700.0f;
		float m_Color[4] = {};

		bool m_IsResizing = false;
		ResizeEdge m_ResizeEdge = ResizeEdge::None;
		float m_ResizeStartX = 0.0f, m_ResizeStartY = 0.0f;
		float m_ResizeStartRectX = 0.0f, m_ResizeStartRectY = 0.0f;
		float m_ResizeStartWidth = 0.0f, m_ResizeStartHeight = 0.0f;
		float m_MinWidth = 10.0f, m_MinHeight = 10.0f;
		int m_EdgeSize = 5;

		float m_CloseButtonSize = 20.0f;
		bool m_IsDragging = false;
		float m_DragStartX = 0.0f, m_DragStartY = 0.0f;
		float m_WindowStartX = 0.0f, m_WindowStartY = 0.0f;
		bool m_IsVisible = true;
		bool m_CloseButtonHovered = false;
		bool m_IsDarkTheme = true;

		std::vector<uiButton*> m_Buttons;

		void DrawTitle() const;
		bool IsInResizeZone(float mx, float my) const;
		void SetResizeCursor(bool isResizeZone);
		ResizeEdge GetResizeEdge(float mx, float my) const;
		void UpdateResizeCursor(ResizeEdge edge);
		void DrawResizeGrip() const;
	};

}