#pragma once
#include "etpch.h"
#include "Core.h"

extern Entry::Application* Entry::CreateApplication();

int main(int argc, char** argv) 
{
    gfxInitDefault();
    Entry::Log::Init();

    ET_CORE_WARN("Initialized Log!");
    ET_INFO("Hello!");


    // Heap allocate; Program can be large, and want to control memory explicitly
    auto app = Entry::CreateApplication();
    app->Run();
    delete app;
}