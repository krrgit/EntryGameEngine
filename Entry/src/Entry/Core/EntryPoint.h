#pragma once
#include "etpch.h"
#include "Core.h"

extern Entry::Application* Entry::CreateApplication();

int main(int argc, char** argv) 
{
    ET_PROFILE_BEGIN_SESSION("Startup", "EntryProfile-Startup.json");
    // Heap allocate; Program can be large, and want to control memory explicitly
    auto app = Entry::CreateApplication();
    ET_PROFILE_END_SESSION();

    ET_PROFILE_BEGIN_SESSION("Runtime", "EntryProfile-Runtime.json");
    app->Run();
    ET_PROFILE_END_SESSION();

    ET_PROFILE_BEGIN_SESSION("Shutdown", "EntryProfile-Shutdown.json");
    delete app;
    ET_PROFILE_END_SESSION();
}