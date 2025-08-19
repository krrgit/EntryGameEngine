#include <Entry.h>
#include <Entry/Core/EntryPoint.h>
#include <Entry/Core/Config.h>

//#include <Platform/Citro3D/Citro3DShader.h>

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"

namespace Entry {
    class Entryway : public Application
    {
    public:
        Entryway()
            : Application("Entryway")
        {
		    PushLayer(new EditorLayer(), ET_WINDOW_TOP);
        }

        ~Entryway()
        {
        }
    };

    Application* CreateApplication() 
    {
        return new Entryway();
    }
}