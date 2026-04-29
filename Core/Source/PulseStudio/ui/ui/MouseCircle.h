#pragma once
#include "PulseStudio/Events/Event.h"

namespace PulseStudio {

	class MouseCircle
	{
	public:
		static MouseCircle& Get();
		void OnUpdate(float deltaTime);
		bool OnEvent(Event& event);

		float GetX() const { return mx; }
		float GetY() const { return my; }
		float GetRadius() const { return m_Radius; }

		bool IsIntersectingRect(float x, float y, float w, float h) const;
	private:
		MouseCircle();
		~MouseCircle();

		float m_Radius = 50.0f;
		float mx = 0.0f, my = 0.0f;
	};

}
