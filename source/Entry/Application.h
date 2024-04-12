#pragma once

#include "Core.h"

namespace Entry {
    class Application{
        public:
        Application();
        virtual ~Application();

        void Run();
    };

    // Define in Client
    Application* CreateApplication();
}