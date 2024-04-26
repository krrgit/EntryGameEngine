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
		PushLayer(new ExampleLayer(), ET_WINDOW_TOP);
		PushOverlay(new Entry::ImGuiLayer(), ET_WINDOW_BOTTOM);
		PushOverlay(new Entry::LogLayer(), ET_WINDOW_BOTTOM);
    }

    ~Sandbox()
    {
    }
};

Entry::Application* Entry::CreateApplication() 
{
    return new Sandbox();
}