#pragma once

#include "Core.h"
#include "Entry/Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Window.h"

namespace Entry {
    class Application{
        public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);
    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    };

    // Define in Client
    Application* CreateApplication();
}