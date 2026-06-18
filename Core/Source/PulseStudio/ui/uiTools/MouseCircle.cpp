#include "pspch.h"
#include "MouseCircle.h"
#include "PulseStudio/Events/MouseEvent.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace PulseStudio
{

	MouseCircle::MouseCircle()
	{
	}
	MouseCircle::~MouseCircle()
	{
	}

	MouseCircle& MouseCircle::Get()
	{
		static MouseCircle instance;
		return instance;
	}

	void MouseCircle::OnUpdate(float deltaTime)
	{
		glColor4f(0.5f, 0.5f, 0.5f, 0.005f);

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(mx, my);
		int segments = 2048; // Number of segments to approximate the circle
		for (int i = 0; i <= segments; ++i)
		{
			float angle = 2.0f * 3.14159265f * i / segments;
			float x = mx + cos(angle) * m_Radius;
			float y = my + sin(angle) * m_Radius;
			glVertex2f(x, y);
		}
		glEnd();
	}

	bool MouseCircle::OnEvent(Event& event)
	{
		if (event.GetEventType() == EventType::MouseMoved)
		{
			MouseMovedEvent& e = (MouseMovedEvent&)event;
			mx = e.GetX();
			my = e.GetY();
			return false;
		}
		return false;
	}

	bool MouseCircle::IsIntersectingRect(float x, float y, float w, float h) const
	{
		float closestX = std::max(x, std::min(mx, x + w));
		float closestY = std::max(y, std::min(my, y + h));
		float dx = mx - closestX;
		float dy = my - closestY;
		return (dx * dx + dy * dy) <= (m_Radius * m_Radius);
	}

}