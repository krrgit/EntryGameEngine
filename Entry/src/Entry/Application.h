#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/LayerStack.h"
#include "Entry/Events/Event.h"
#include "Entry/Events/ApplicationEvent.h"

namespace Entry {
    class Application{
        public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline static Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_Window; }
    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;
    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}