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
        Entryway(AppProperties* appProps)
            : Application(appProps)
        {
		    PushLayer(new EditorLayer(), ET_WINDOW_TOP);
        }

        ~Entryway()
        {
        }
    };

    Application* CreateApplication() 
    {
        WindowProps mainEditorWindow("Entryway", 1280, 720, 0, false, false);
        WindowProps touchWindow(WindowProps::WINDOW_3DS_BOTTOM);
        touchWindow.Title = "Entryway: Bottom Screen";

        AppProperties appProps;
        appProps.windowProps.push_back(mainEditorWindow);
        appProps.windowProps.push_back(touchWindow);

        appProps.ImGuiWindow = ET_WINDOW_TOP;

        return new Entryway(&appProps);
    }
}