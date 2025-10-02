#include <Entry.h>
#include <Entry/Core/EntryPoint.h>
#include <Entry/Core/Config.h>

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Sandbox3D.h"

class Sandbox : public Entry::Application
{
public:
    Sandbox()
    {
		PushLayer(new Sandbox3D(), ET_WINDOW_TOP);
#ifdef ET_PLATFORM_3DS
    #ifdef ET_LOG_ENABLED
		    PushOverlay(new Entry::LogLayer(), ET_WINDOW_TOP);
    #endif
#endif
    }

    ~Sandbox()
    {
    }
};

Entry::Application* Entry::CreateApplication() 
{
    return new Sandbox();
}