

#include "etpch.h"

#include <imgui.h>
#include <imgui_sw.h>

#include "ImGuiCitro3DRenderer.h"
#include "Entry/Core/Application.h"
#include "Platform/Citro3D/Citro3DWindow.h"

using namespace Entry;

struct ImGui_ImplC3D_Data {
	C3D_RenderTarget*			m_RenderTarget;
	uint16_t					m_Width, m_Height;
	imgui_sw::SwOptions			sw_options;

	ImGui_ImplC3D_Data() {} //memset((void*)this, 0, sizeof(*this));
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplC3D_Data* ImGui_ImplC3D_GetBackendData()
{
	return ImGui::GetCurrentContext() ? (ImGui_ImplC3D_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}


bool ImGui_ImplC3D_Init()
{
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

	// Setup backend capabilities flags
	ImGui_ImplC3D_Data* bd = IM_NEW(ImGui_ImplC3D_Data)();
	io.BackendRendererUserData = (void*)bd;

	// Set render target and parameters
	Citro3DWindow* citroWindow = static_cast<Citro3DWindow*>(&Application::Get().GetWindow());
	bd->m_RenderTarget = citroWindow->GetRenderTarget();
	bd->m_Width = citroWindow->GetWidth();
	bd->m_Height = citroWindow->GetHeight();
	
	imgui_sw::bind_imgui_painting();
	imgui_sw::make_style_fast();

	return true;
}
bool ImGui_ImplC3D_Shutdown()
{
	ImGui_ImplC3D_Data* bd = ImGui_ImplC3D_GetBackendData();
	IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
	ImGuiIO& io = ImGui::GetIO();

	io.BackendRendererUserData = nullptr;
	IM_DELETE(bd);

	imgui_sw::unbind_imgui_painting();
	return true;
}


bool ImGui_ImplC3D_NewFrame()
{
	return true;
}

void ImGui_ImplC3D_RenderDrawData() {
	ImGui_ImplC3D_Data* bd = ImGui_ImplC3D_GetBackendData();

	imgui_sw::paint_imgui(bd->m_Width, bd->m_Height, bd->sw_options);
}
