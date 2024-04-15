#include "etpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_sw.h"
#include <time.h>
#include "Platform/Citro3D/Citro3DWindow.h"
#include "Entry/Application.h"

namespace Entry {
	typedef enum {
		BTN_A = 0,       ///< A
		BTN_B ,       ///< B
		BTN_SELECT,       ///< Select
		BTN_START,       ///< Start
		BTN_DRIGHT,       ///< D-Pad Right
		BTN_DLEFT,       ///< D-Pad Left
		BTN_DUP,       ///< D-Pad Up
		BTN_DDOWN,       ///< D-Pad Down
		BTN_R,       ///< R
		BTN_L,       ///< L
		BTN_X,      ///< X
		BTN_Y,      ///< Y
		BTN_ZL = 14,      ///< ZL (New 3DS only)
		BTN_ZR,      ///< ZR (New 3DS only)
		BTN_TOUCH = 20,      ///< Touch (Not actually provided by HID)
		BTN_CSTICK_RIGHT = 24, ///< C-Stick Right (New 3DS only)
		BTN_CSTICK_LEFT, ///< C-Stick Left (New 3DS only)
		BTN_CSTICK_UP, ///< C-Stick Up (New 3DS only)
		BTN_CSTICK_DOWN, ///< C-Stick Down (New 3DS only)
		BTN_CPAD_RIGHT,   ///< Circle Pad Right
		BTN_CPAD_LEFT,   ///< Circle Pad Left
		BTN_CPAD_UP,   ///< Circle Pad Up
		BTN_CPAD_DOWN 
	} KeyCode;

	ImGuiNavInput_ MapKeyCodeToImGuiNavInput(KeyCode keycode) {
		switch (keycode) {
		case BTN_A:			return ImGuiNavInput_Activate;	// A
		case BTN_B:			return ImGuiNavInput_Cancel;	// B
		case BTN_Y:			return ImGuiNavInput_Menu;	// X
		case BTN_X:			return ImGuiNavInput_Input; // Y
		case BTN_DLEFT:		return ImGuiNavInput_DpadLeft;
		case BTN_DRIGHT:	return ImGuiNavInput_DpadRight;
		case BTN_DUP:		return ImGuiNavInput_DpadUp;
		case BTN_DDOWN:		return ImGuiNavInput_DpadDown;
		case BTN_L:			return ImGuiNavInput_FocusPrev; // L
		case BTN_R:			return ImGuiNavInput_FocusNext; // R
		default:
			break;
		}
		return ImGuiNavInput_FocusNext;

	}

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
		//ImGui::SetMouseCursor()
		
		imgui_sw::bind_imgui_painting();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		io.MouseDrawCursor = true;

		memset(io.NavInputs, 0, sizeof(io.NavInputs));
		m_PixelBuffer = std::vector<uint32_t>(m_Width * m_Height, 0);
	}

	void ImGuiLayer::OnDetach()
	{
		imgui_sw::unbind_imgui_painting();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate()
	{
		imgui_sw::SwOptions sw_options;
		imgui_sw::make_style_fast();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)m_Width, (float)m_Height);

		time_t f_time = time(NULL);
		io.DeltaTime = m_Time > 0 ? difftime(f_time, m_Time) : (1.0f / 60.0f);
		m_Time = f_time;

		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();

		// I/O
		io.MouseDown[0] = false; // Reset mouse button down

		// fill clear (this could be any previous rendering)
		std::fill_n(m_PixelBuffer.data(), m_Width * m_Height, 0x00000000u);

		// overlay the GUI
		imgui_sw::paint_imgui(m_PixelBuffer.data(), m_Width, m_Height, sw_options);

		for (u32 x = 0; x < m_Width; x++)
		{
			for (u32 y = 0; y < m_Height; y++)
			{
				u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 4;
				u32 srcPos = (y * m_Width + x) * 4;
				memcpy(&((u8*)m_Image.tex->data)[dstPos], &((u8*)m_PixelBuffer.data())[srcPos], 4);
			}
		}

		C2D_SceneBegin(m_RenderTarget);
		C2D_DrawImageAt(m_Image, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
	}

	void ImGuiLayer::OnEvent(Event& event) 
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<ScreenTouchedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnScreenTouchedEvent));
		dispatcher.Dispatch<ScreenReleasedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnScreenReleasedEvent));
		dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<CirclePadEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnCirclePadMovedEvent));
	}

	bool ImGuiLayer::OnScreenTouchedEvent(ScreenTouchedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = true;
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}
	bool ImGuiLayer::OnScreenReleasedEvent(ScreenReleasedEvent& e)
	{
		// Mouse doesn't work when this function is implemented here
		return false;
	}
	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.NavInputs[MapKeyCodeToImGuiNavInput((KeyCode)e.GetKeyCode())] = 1.0f;
		return false;
	}
	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnCirclePadMovedEvent(CirclePadEvent& e)
	{
		return false;
	}
}