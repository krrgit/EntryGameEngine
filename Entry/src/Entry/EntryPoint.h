#pragma once
#include <3ds.h>
#include "Core.h"

extern Entry::Application* Entry::CreateApplication();

int main(int argc, char** argv) 
{
    gfxInitDefault();
    Entry::Log::Init();

    Entry::Log::GetCoreLogger()->warn("Initialized Log!");
    Entry::Log::GetClientLogger()->info("Hello!");


    // Heap allocate; Program can be large, and want to control memory explicitly
    auto app = Entry::CreateApplication();
    app->Run();
    delete app;
}