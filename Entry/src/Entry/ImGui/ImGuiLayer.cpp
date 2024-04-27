#include "etpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/Citro3D/ImGuiCitro3DRenderer.h"

#include "KeyCodes.h"
#include "Entry/Application.h"

namespace Entry {

	static ImGuiNavInput_ MapKeyCodeToImGuiNavInput(uint32_t keycode) {
		switch (keycode) {
		case ET_KEY_A:			return ImGuiNavInput_Activate;
		case ET_KEY_B:			return ImGuiNavInput_Cancel;
		case ET_KEY_Y:			return ImGuiNavInput_Menu;
		case ET_KEY_X:			return ImGuiNavInput_Input;
		case ET_KEY_DLEFT:		return ImGuiNavInput_DpadLeft;
		case ET_KEY_DRIGHT:		return ImGuiNavInput_DpadRight;
		case ET_KEY_DUP:		return ImGuiNavInput_DpadUp;
		case ET_KEY_DDOWN:		return ImGuiNavInput_DpadDown;
		case ET_KEY_L:			return ImGuiNavInput_FocusPrev;
		case ET_KEY_R:			return ImGuiNavInput_FocusNext;
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
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		io.MouseDrawCursor = true;

		ImGui_ImplC3D_Init();
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplC3D_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate(Timestep ts)
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
		io.DeltaTime = ts;

		ImGui_ImplC3D_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();

		// I/O
		io.MouseDown[0] = false; // Reset io.MouseDown

		ImGui_ImplC3D_RenderDrawData();
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

	bool ImGuiLayer::OnScreenTouchedEvent(ScreenTouchedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = true;
		io.MousePos = ImVec2(e.GetX(), e.GetY());
		return false;
	}
	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.NavInputs[MapKeyCodeToImGuiNavInput(e.GetKeyCode())] = 1.0f;
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