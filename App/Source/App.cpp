#include <Pulse.h>
#include <iostream>

#include "PulseCode/Events/KeyEvent.h"

class ExampleLayer : public PulseCode::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate(float deltaTime) override
	{
	}

	bool OnEvent(PulseCode::Event& event) override
	{
		return false;
	}
};

class SandboxApp : public PulseCode::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new PulseCode::uiLayer());
	}
	~SandboxApp()
	{
	}
};

PulseCode::Application* PulseCode::CreateApplication()
{
	return new SandboxApp();
}

int main()
{
	PulseCode::Application* app = PulseCode::CreateApplication();

	app->Run();

	delete app;
	return 0;
}
