

#include "etpch.h"

#include <imgui.h>
#include <imgui_sw.h>

#include "ImGuiCitro3DRenderer.h"
#include "Entry/Application.h"
#include "Platform/Citro3D/Citro3DWindow.h"

using namespace Entry;

struct ImGui_ImplC3D_Data {
	C3D_RenderTarget*			m_RenderTarget;
	C3D_Tex						m_tex;
	Tex3DS_SubTexture			m_subt3x;// = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
	uint32_t*					m_PixelBuffer;
	C2D_Image					m_Image;
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

	// Setup Citro2D, textures and images
	bd->m_subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
	bd->m_Image = (C2D_Image){ &(bd->m_tex), &(bd->m_subt3x) };
	C3D_TexInitVRAM(&bd->m_tex, 512, 256, GPU_RGBA8);
	C3D_TexSetFilter(&bd->m_tex, GPU_LINEAR, GPU_LINEAR);
	C3D_TexSetWrap(&bd->m_tex, GPU_REPEAT, GPU_REPEAT);

	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Set render target and parameters
	Citro3DWindow* citroWindow = static_cast<Citro3DWindow*>(&Application::Get().GetWindow());
	bd->m_RenderTarget = citroWindow->GetRenderTarget();
	bd->m_Width = citroWindow->GetWidth();
	bd->m_Height = citroWindow->GetHeight();

	bd->m_PixelBuffer = (uint32_t*)malloc(bd->m_Width * bd->m_Height * sizeof(uint32_t));
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

	uint32_t* texData = (uint32_t*)bd->m_Image.tex->data;
	uint16_t texWidth = bd->m_Image.tex->width;
	uint16_t texHeight = bd->m_Image.tex->height;

	// Clear buffer by setting alpha to 0
	static const size_t size = texWidth * texHeight;
	uint8_t* textData8 = (uint8_t*)bd->m_Image.tex->data;
	for (size_t i = 0; i < size; ++i) {
		textData8[i * 4] = 0x00 << IM_COL32_A_SHIFT;
	}

	imgui_sw::paint_imgui(texData, bd->m_Width, bd->m_Height, bd->sw_options);
	return true;
}


void ImGui_ImplC3D_RenderDrawData() {
	ImGui_ImplC3D_Data* bd = ImGui_ImplC3D_GetBackendData();

	C2D_SceneBegin(bd->m_RenderTarget);
	C2D_DrawImageAt(bd->m_Image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}
