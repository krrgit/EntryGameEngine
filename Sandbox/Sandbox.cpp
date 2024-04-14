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
		ET_TRACE("{0}", event);
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