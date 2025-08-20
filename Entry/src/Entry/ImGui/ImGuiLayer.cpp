#include "etpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>

#include "Entry/Core/KeyCodes.h"
#include "Entry/Core/Application.h"

// Shared across platforms
namespace Entry
{
	void ImGuiLayer::OnImGuiRender()
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}
}

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/ImGuiCitro3DRenderer.h"

#define TOUCH_INPUT_SHIFT 40
namespace Entry {

	static ImGuiKey MapKeyCodeToImGuiNavInput(KeyCode keycode) {
		switch (keycode) {
		case ET_PAD_A:			return ImGuiKey_GamepadFaceRight;
		case ET_PAD_B:			return ImGuiKey_GamepadFaceDown;
		case ET_PAD_Y:			return ImGuiKey_GamepadFaceLeft;
		case ET_PAD_X:			return ImGuiKey_GamepadFaceUp;
		case ET_PAD_DLEFT:		return ImGuiKey_GamepadDpadLeft;
		case ET_PAD_DRIGHT:		return ImGuiKey_GamepadDpadRight;
		case ET_PAD_DUP:		return ImGuiKey_GamepadDpadUp;
		case ET_PAD_DDOWN:		return ImGuiKey_GamepadDpadDown;
		case ET_PAD_L:			return ImGuiKey_GamepadL1;
		case ET_PAD_R:			return ImGuiKey_GamepadR1;
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
		
		// Align touch input horizontally with top screen
		m_TouchInputShift = (io.DisplaySize.x > 320) ? TOUCH_INPUT_SHIFT : 0;

		{
			ET_PROFILE_SCOPE("ImGui_ImplC3D_Init");
			ImGui_ImplC3D_Init();
		}
	}

	void ImGuiLayer::OnDetach()
	{
		ET_PROFILE_FUNCTION();
		ImGui_ImplC3D_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& event) 
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<ScreenTouchedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnScreenTouchedEvent));
		dispatcher.Dispatch<ButtonPressedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnButtonPressedEvent));
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

	void ImGuiLayer::End()
	{
		ET_PROFILE_FUNCTION();

		ImGui::Render();
		ImGui_ImplC3D_RenderDrawData();

		// I/O
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = false;
	}

	bool ImGuiLayer::OnScreenTouchedEvent(ScreenTouchedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = true;
		io.MousePos = ImVec2(e.GetX() + m_TouchInputShift, e.GetY());
		return false;
	}
	bool ImGuiLayer::OnScreenReleasedEvent(ScreenReleasedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnButtonPressedEvent(ButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(MapKeyCodeToImGuiNavInput(e.GetKeyCode()), true);
		return false;
	}
	bool ImGuiLayer::OnButtonReleasedEvent(ButtonReleasedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnCirclePadMovedEvent(CirclePadEvent& e)
	{
		return false;
	}
}

#endif

#ifdef ET_PLATFORM_WINDOWS
#include "Entry/Events/ApplicationEvent.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <Glad/glad.h>


namespace Entry
{

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

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		m_Window = &app.GetWindow();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_Window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ET_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin(Timestep ts)
	{
		ET_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ET_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		//Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)m_Window->GetWidth(), (float)m_Window->GetHeight());

		glfwMakeContextCurrent((GLFWwindow*)m_Window->GetNativeWindow());
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = (GLFWwindow*)m_Window->GetNativeWindow();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_BlockEvents) {
			ImGuiIO& io = ImGui::GetIO();
			event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<MouseButtonPressedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
			dispatcher.Dispatch<MouseButtonReleasedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
			dispatcher.Dispatch<MouseMovedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
			dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
			dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
			dispatcher.Dispatch<KeyTypedEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
			dispatcher.Dispatch<WindowResizeEvent>(ET_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[(int)e.GetMouseButton()] = true;

		return e.Handled;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[(int)e.GetMouseButton()] = false;

		return e.Handled;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return e.Handled;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.GetXOffset();
		io.MouseWheel += e.GetYOffset();

		return e.Handled;
	}


	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = (uint32_t)e.GetKeyCode() - ImGuiKey_NamedKey_BEGIN;
		io.KeysData[keycode].Down = true;

		io.KeyCtrl  = io.KeysData[GLFW_KEY_LEFT_CONTROL].Down || io.KeysData[GLFW_KEY_RIGHT_CONTROL].Down;
		io.KeyShift = io.KeysData[GLFW_KEY_LEFT_SHIFT].Down || io.KeysData[GLFW_KEY_RIGHT_SHIFT].Down;
		io.KeyAlt   = io.KeysData[GLFW_KEY_LEFT_ALT].Down || io.KeysData[GLFW_KEY_RIGHT_ALT].Down;
		io.KeySuper = io.KeysData[GLFW_KEY_LEFT_SUPER].Down || io.KeysData[GLFW_KEY_RIGHT_SUPER].Down;
 		
		return e.Handled;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysData[(uint32_t)e.GetKeyCode() - ImGuiKey_NamedKey_BEGIN].Down = false;
		return e.Handled;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = (int)e.GetKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);
		
		return e.Handled;
	}

	bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return e.Handled;
	}

	bool ImGuiLayer::OnScreenTouchedEvent(ScreenTouchedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnScreenReleasedEvent(ScreenReleasedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnCirclePadMovedEvent(CirclePadEvent& e)
	{
		return false;
	}
}
#endif