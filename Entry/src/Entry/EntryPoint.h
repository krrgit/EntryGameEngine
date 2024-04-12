#pragma once
#include <3ds.h>
#include "Core.h"

extern Entry::Application* Entry::CreateApplication();

int main(int argc, char** argv) 
{
    gfxInitDefault();
    Entry::Log::Init();

    ET_CORE_WARN("Initialized Log!");
    int a = 5;
    ET_INFO("Hello! Var = %d", a);
    ET_FATAL("FATAL TEXT DEMO");

    // Heap allocate; can be large, and want to control memory explicitly
    auto app = Entry::CreateApplication();
    app->Run();
    delete app;
}