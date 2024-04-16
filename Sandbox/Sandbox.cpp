#include <Entry.h>

class ExampleLayer : public Entry::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//ET_INFO("ExampleLayer::Update");
	}

	void OnEvent(Entry::Event& event) override
	{
		if (Entry::Input::IsKeyPressed(ET_KEY_A))
			ET_TRACE("A Button is pressed!");
	}

};

class Sandbox : public Entry::Application
{
public:
    Sandbox()
    {
		PushLayer(new ExampleLayer());
		PushOverlay(new Entry::ImGuiLayer());
    }

    ~Sandbox()
    {
    }
};

Entry::Application* Entry::CreateApplication() 
{
    return new Sandbox();
}