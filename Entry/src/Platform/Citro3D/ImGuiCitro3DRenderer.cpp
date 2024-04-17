

#include "etpch.h"

#include <imgui.h>
#include <imgui_sw.h>

#include "ImGuiCitro3DRenderer.h"
#include "Entry/Application.h"
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

<<<<<<< Updated upstream
	// Setup Citro2D, textures and images
	bd->m_subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
	bd->m_Image = (C2D_Image){ &(bd->m_tex), &(bd->m_subt3x) };
	C3D_TexInit(&bd->m_tex, 512, 256, GPU_RGBA8);
	C3D_TexSetFilter(&bd->m_tex, GPU_LINEAR, GPU_LINEAR);
	C3D_TexSetWrap(&bd->m_tex, GPU_REPEAT, GPU_REPEAT);

=======
>>>>>>> Stashed changes
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Set render target and parameters
	Citro3DWindow* citroWindow = static_cast<Citro3DWindow*>(&Application::Get().GetWindow());
	bd->m_RenderTarget = citroWindow->GetRenderTarget();
	bd->m_Width = citroWindow->GetWidth();
	bd->m_Height = citroWindow->GetHeight();

	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	
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
	ImGui_ImplC3D_Data* bd = ImGui_ImplC3D_GetBackendData();
	std::fill_n(bd->m_PixelBuffer, bd->m_Width * bd->m_Height, 0x00000000u);

<<<<<<< Updated upstream
	imgui_sw::paint_imgui(bd->m_PixelBuffer, bd->m_Width, bd->m_Height, bd->sw_options);

	for (u32 x = 0; x < bd->m_Width; x++)
	{
		for (u32 y = 0; y < bd->m_Height; y++)
		{
			u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 4;
			u32 srcPos = (y * bd->m_Width + x) * 4;
			memcpy(&((u8*)bd->m_Image.tex->data)[dstPos], &((u8*)bd->m_PixelBuffer)[srcPos], 4);
		}
	}

=======
	imgui_sw::paint_imgui(bd->m_Width, bd->m_Height, bd->sw_options);
>>>>>>> Stashed changes
	return true;
}

void ImGui_ImplC3D_RenderDrawData() {
	ImGui_ImplC3D_Data* bd = ImGui_ImplC3D_GetBackendData();

	C2D_SceneBegin(bd->m_RenderTarget);
	//C2D_DrawImageAt(bd->m_Image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}
