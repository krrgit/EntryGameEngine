#pragma once

#include "Core.h"

#include "Window.h"
#include "Entry/LayerStack.h"
#include "Entry/Events/Event.h"
#include "Entry/Events/ApplicationEvent.h"

#define ET_WINDOW_TOP 0
#define ET_WINDOW_BOTTOM 1

namespace Entry {

    class Application{
        public:
            Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer, int window);
        void PushOverlay(Layer* layer, int window);

        inline static Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_CurrentWindow; }
    private:
        Window* m_CurrentWindow;
        std::unique_ptr<Window> m_WindowTop;
        std::unique_ptr<Window> m_WindowBottom;
        bool m_Running = true;
        LayerStack m_LayerStack;
    private:
        static Application* s_Instance;
    };

    // Define in Client
    Application* CreateApplication();
}