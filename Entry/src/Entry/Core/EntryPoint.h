#pragma once
#include "etpch.h"
#include "Core.h"

extern Entry::Application* Entry::CreateApplication();

int main(int argc, char** argv) 
{

    // Heap allocate; Program can be large, and want to control memory explicitly
    auto app = Entry::CreateApplication();
    app->Run();
    delete app;
}