#pragma once

#include "Core.h"

extern Entry::Application* Entry::CreateApplication();

int main(int argc, char** argv) 
{
    // Heap allocate; can be large, and want to control memory explicitly
    auto app = Entry::CreateApplication();
    app->Run();
    delete app;
}