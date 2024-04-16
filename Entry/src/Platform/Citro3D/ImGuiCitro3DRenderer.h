#pragma once

#include "imgui.h"
#include "imgui_sw.h"
#include <time.h>
#include "Platform/Citro3D/Citro3DWindow.h"

bool ImGui_ImplC3D_Init();
bool ImGui_ImplC3D_Shutdown();
bool ImGui_ImplC3D_NewFrame();
void ImGui_ImplC3D_RenderDrawData();