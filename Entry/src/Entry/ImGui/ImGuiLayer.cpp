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

		C3D_Tex* tex = (C3D_Tex*)malloc(sizeof(C3D_Tex));
		//static const Tex3DS_SubTexture subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };

		m_Image = (C2D_Image){ tex, &m_subt3x };
		C3D_TexInit(m_Image.tex, 512, 256, GPU_RGBA8);
		C3D_TexSetFilter(m_Image.tex, GPU_LINEAR, GPU_LINEAR);
		C3D_TexSetWrap(m_Image.tex, GPU_REPEAT, GPU_REPEAT);

		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
		C2D_Prepare();

		Window* window = &Application::Get().GetWindow();
		Citro3DWindow* citroWindow = static_cast<Citro3DWindow*>(window);
		m_RenderTarget = citroWindow->GetRenderTarget();
		m_Width = citroWindow->GetWidth();
		m_Height = citroWindow->GetHeight();
		 

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
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
		std::vector<uint32_t> m_pixelBuffer(m_Width * m_Height, 0);

		imgui_sw::SwOptions sw_options;
		imgui_sw::make_style_fast();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		io.DisplaySize = ImVec2((float)m_Width, (float)m_Height);
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.MouseDrawCursor = true;

		time_t f_time = time(NULL);
		io.DeltaTime = m_Time > 0 ? difftime(f_time, m_Time) : (1.0f / 60.0f);
		m_Time = f_time;

		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();


		// I/O
		touchPosition touch;
		u32 kHeld = keysDown();
		memset(io.NavInputs, 0, sizeof(io.NavInputs));
#define MAP_BUTTON(NAV, BUTTON)       { if (kHeld & BUTTON) io.NavInputs[NAV] = 1.0f; }
		MAP_BUTTON(ImGuiNavInput_Activate, KEY_A);
		MAP_BUTTON(ImGuiNavInput_Cancel, KEY_B);
		MAP_BUTTON(ImGuiNavInput_Menu, KEY_Y);
		MAP_BUTTON(ImGuiNavInput_Input, KEY_X);
		MAP_BUTTON(ImGuiNavInput_DpadLeft, KEY_DLEFT);
		MAP_BUTTON(ImGuiNavInput_DpadRight, KEY_DRIGHT);
		MAP_BUTTON(ImGuiNavInput_DpadUp, KEY_DUP);
		MAP_BUTTON(ImGuiNavInput_DpadDown, KEY_DDOWN);
		MAP_BUTTON(ImGuiNavInput_FocusPrev, KEY_L);
		MAP_BUTTON(ImGuiNavInput_FocusNext, KEY_R);
		//MAP_BUTTON(ImGuiNavInput_TweakSlow,   KEY_L);
		//MAP_BUTTON(ImGuiNavInput_TweakFast,   KEY_R);
#undef MAP_BUTTON
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		hidTouchRead(&touch);

		if (touch.px && touch.py)
		{
			io.MouseDown[0] = true;
			io.MousePos = ImVec2(touch.px, touch.py);
		}
		else
			io.MouseDown[0] = false;

		// fill clear (this could be any previous rendering)
		std::fill_n(m_pixelBuffer.data(), m_Width * m_Height, 0x00000000u);

		// overlay the GUI
		imgui_sw::paint_imgui(m_pixelBuffer.data(), m_Width, m_Height, sw_options);

		for (u32 x = 0; x < m_Width; x++)
		{
			for (u32 y = 0; y < m_Height; y++)
			{
				u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 4;
				u32 srcPos = (y * m_Width + x) * 4;
				memcpy(&((u8*)m_Image.tex->data)[dstPos], &((u8*)m_pixelBuffer.data())[srcPos], 4);
			}
		}

		C2D_SceneBegin(m_RenderTarget);
		C2D_DrawImageAt(m_Image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}

	void ImGuiLayer::OnEvent(Event& event) 
	{
	}
}