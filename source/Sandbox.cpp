#include <Entry.h>

class Sandbox : public Entry::Application
{
public:
    Sandbox()
    {
    }

    ~Sandbox()
    {
    }
};

Entry::Application* Entry::CreateApplication() 
{
    return new Sandbox();
}