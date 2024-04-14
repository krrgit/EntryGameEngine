#include "etpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_sw.h"
#include <time.h>
#include "Platform/Citro3D/Citro3DWindow.h"
#include "Entry/Application.h"

namespace Entry {
	ImGuiLayer::ImGuiLayer() 
		: Layer("ImGuiLayer") 
	{
	}

	ImGuiLayer::~ImGuiLayer() 
	{

	}

	void ImGuiLayer::OnAttach()
	{
		u16 width = 320, height = 240;

		C3D_Tex* tex = (C3D_Tex*)malloc(sizeof(C3D_Tex));
		//static const Tex3DS_SubTexture subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };

		image = (C2D_Image){ tex, &subt3x };
		C3D_TexInit(image.tex, 512, 256, GPU_RGBA8);
		C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
		C3D_TexSetWrap(image.tex, GPU_REPEAT, GPU_REPEAT);

		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
		C2D_Prepare();

		Window* window = &Application::Get().GetWindow();
		Citro3DWindow* citroWindow = static_cast<Citro3DWindow*>(window);
		m_renderTarget = citroWindow->GetRenderTarget();
		 

		ImGui::CreateContext();
		//ImGui::SetMouseCursor()
		
		imgui_sw::bind_imgui_painting();
	}

	void ImGuiLayer::OnDetach()
	{
		imgui_sw::unbind_imgui_painting();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate() 
	{
		static u16 width = 320, height = 240;
		std::vector<uint32_t> pixel_buffer(width * height, 0);

		imgui_sw::SwOptions sw_options;
		imgui_sw::make_style_fast();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		io.DisplaySize = ImVec2((float)width, (float)height);
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.MouseDrawCursor = true;

		time_t f_time = time(NULL);
		io.DeltaTime = m_Time > 0 ? difftime(f_time, m_Time) : (1.0f / 60.0f);
		m_Time = f_time;

		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();

		// fill gray (this could be any previous rendering)
		std::fill_n(pixel_buffer.data(), width * height, 0x19191900u);

		// overlay the GUI
		imgui_sw::paint_imgui(pixel_buffer.data(), width, height, sw_options);

		for (u32 x = 0; x < width; x++)
		{
			for (u32 y = 0; y < height; y++)
			{
				u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 4;
				u32 srcPos = (y * 320 + x) * 4;
				memcpy(&((u8*)image.tex->data)[dstPos], &((u8*)pixel_buffer.data())[srcPos], 4);
			}
		}

		C2D_SceneBegin(m_renderTarget);
		C2D_DrawImageAt(image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}

	void ImGuiLayer::OnEvent(Event& event) 
	{
	}
}