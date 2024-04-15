#include "etpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/Citro3D/ImGuiCitro3DRenderer.h"

#include "Entry/Application.h"
#include <time.h>


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
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		io.MouseDrawCursor = true;
		
		ImGui_ImplC3D_InitForCitro3D();
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplC3D_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		time_t f_time = time(NULL);
		io.DeltaTime = m_Time > 0 ? difftime(f_time, m_Time) : (1.0f / 60.0f);
		m_Time = f_time;

		ImGui_ImplC3D_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();

		// I/O
		io.MouseDown[0] = false; // Reset mouse button down

		ImGui_ImplC3D_RenderDrawData();
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