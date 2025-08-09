#include "etpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>

#include "Entry/Core/KeyCodes.h"
#include "Entry/Core/Application.h"

#ifdef ET_PLATFORM_3DS
	#include "Platform/Citro3D/ImGuiCitro3DRenderer.h"
#endif

namespace Entry {

	static ImGuiKey MapKeyCodeToImGuiNavInput(uint32_t keycode) {
		switch (keycode) {
		case ET_KEY_A:			return ImGuiKey_GamepadFaceRight;
		case ET_KEY_B:			return ImGuiKey_GamepadFaceDown;
		case ET_KEY_Y:			return ImGuiKey_GamepadFaceLeft;
		case ET_KEY_X:			return ImGuiKey_GamepadFaceUp;
		case ET_KEY_DLEFT:		return ImGuiKey_GamepadDpadLeft;
		case ET_KEY_DRIGHT:		return ImGuiKey_GamepadDpadRight;
		case ET_KEY_DUP:		return ImGuiKey_GamepadDpadUp;
		case ET_KEY_DDOWN:		return ImGuiKey_GamepadDpadDown;
		case ET_KEY_L:			return ImGuiKey_GamepadL1;
		case ET_KEY_R:			return ImGuiKey_GamepadR1;
		default:
			break;
		}
		return ImGuiKey_None;

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
		ET_PROFILE_FUNCTION();
		{
			ET_PROFILE_SCOPE("ImGui::CreateContext");
			ImGui::CreateContext();
		}
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.MouseDrawCursor = true;

		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		{
			ET_PROFILE_SCOPE("ImGui_ImplC3D_Init");
#ifdef ET_PLATFORM_3DS
			ImGui_ImplC3D_Init();
#endif
		}
	}

	void ImGuiLayer::OnDetach()
	{
		ET_PROFILE_FUNCTION();
#ifdef ET_PLATFORM_3DS
		ImGui_ImplC3D_Shutdown();
#endif
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& event) 
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<ScreenTouchedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnScreenTouchedEvent));
		dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		// Touch Doesn't work properly when ScreenReleasedEvent implemented
		//dispatcher.Dispatch<ScreenReleasedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnScreenReleasedEvent));
		//dispatcher.Dispatch<KeyReleasedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		//dispatcher.Dispatch<CirclePadEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnCirclePadMovedEvent));
	}

	void ImGuiLayer::Begin(Timestep ts)
	{
		ET_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = ts;

		//ImGui_ImplC3D_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		 //static bool show = false;
		 //ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::End()
	{
		ET_PROFILE_FUNCTION();

		ImGui::Render();
#ifdef ET_PLATFORM_3DS
		ImGui_ImplC3D_RenderDrawData();
#endif

		// I/O
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = false;
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
		return false;
	}
	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(MapKeyCodeToImGuiNavInput(e.GetKeyCode()), true);
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