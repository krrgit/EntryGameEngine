#include "etpch.h"
#include "Application.h"

#include "Input.h"
#include "Entry/Renderer/Renderer.h"


#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
#define CONSOLE_SCREEN SCREEN_NULL

    Application* Application::s_Instance = nullptr;

    typedef struct { float x, y, z; } vertex;


    Application::Application()
        : m_Camera(-1.0f, 1.0f, -1.0f, 1.0f )
    {

        gfxInitDefault();
        Log::Init();

        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
        C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

        WindowProps topProps("Top", 400, 240, GFX_TOP);
        m_WindowTop = std::unique_ptr<Window>(Window::Create(topProps));
        m_CurrentWindow = m_WindowTop.get();

        RenderCommand::SetClearColor(0x68B0D8FF);


        WindowProps props("Bottom", 320, 240, GFX_BOTTOM);
        m_WindowBottom = std::unique_ptr<Window>(Window::Create(props));
        m_WindowBottom->SetEventCallback(BIND_EVENT_FN(OnEvent));

        m_CurrentWindow = m_WindowBottom.get();
        RenderCommand::SetClearColor(0x191919FF);

        m_Shader.reset(new Shader(0));

        m_VertexArray.reset(VertexArray::Create());
        
        float vertices[] = {
            0.0f,  0.5f,  0.2f, 0.8f, 0.8f, 0.2f, 1.0f,
            -0.5f, -0.5f, 0.2f, 0.8f, 0.2f, 0.8f, 1.0f,
            0.5f,  -0.5f, 0.2f, 0.2f, 0.2f, 0.8f, 1.0f,
        };

        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));        

        m_VertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"}
         });

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        uint16_t indices[] = { 0, 1, 2 };
        m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint16_t)));

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_SquareVA.reset(VertexArray::Create());

        float squareVertices[4 * 3] =
        {
            0.75f,  0.75f,  0.0f,
           -0.75f,  0.75f,  0.0f,
           -0.75f, -0.75f,  0.0f,
            0.75f, -0.75f,  0.0f,
        };


        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
         });
        m_SquareVA->AddVertexBuffer(squareVB);

        m_BlueShader.reset(new Shader(1));

        u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        // Configure the first fragment shading substage to just pass through the vertex color
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
    }


    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer, int window)
    {
        if (window == 0)
            m_WindowTop->PushLayer(layer);
        else if (window == 1)
            m_WindowBottom->PushLayer(layer);
    }

    void Application::PushOverlay(Layer* layer, int window)
    {
        if (window == 0)
            m_WindowTop->PushOverlay(layer);
        else if (window == 1)
            m_WindowBottom->PushOverlay(layer);
    }

    void Application::OnEvent(Event& e)
    {
        m_WindowTop->OnEvent(e);
        m_WindowBottom->OnEvent(e);
    }

    void Application::Run()
    {
        
        while (aptMainLoop() && m_Running) {
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

            m_CurrentWindow = m_WindowTop.get();
            m_CurrentWindow->FrameDrawOn();

            //RenderComand::Clear();

            glm::vec2 cp = Input::GetJoystickPos();

            glm::vec3 forward = m_Camera.forward;
            forward.y = 0;
            glm::vec3 right = m_Camera.right;
            right.y = 0;
            m_CamPos = m_CamPos + forward * (cp.y * 0.1f) + (right * (cp.x * 0.1f));
            int LandR = (Input::IsKeyPressed(ET_KEY_R) ? 1 : 0) - (Input::IsKeyPressed(ET_KEY_L) ? 1 : 0);
            m_CamPos.y += LandR * 0.05f;

            int cStickX = (Input::IsKeyPressed(ET_KEY_CSTICK_LEFT) ? 1 : 0) - (Input::IsKeyPressed(ET_KEY_CSTICK_RIGHT) ? 1 : 0);
            int cStickY = (Input::IsKeyPressed(ET_KEY_CSTICK_UP) ? 1 : 0) - (Input::IsKeyPressed(ET_KEY_CSTICK_DOWN) ? 1 : 0);

            m_CamRot = glm::vec4(m_CamRot.x + (cStickY * 2.0f), m_CamRot.y + (cStickX * 2.0f), m_CamRot.z, m_CamRot.w);

            m_Camera.SetPosition(m_CamPos);
            m_Camera.SetRotation(m_CamRot);

            Renderer::BeginScene(m_Camera);

            Renderer::Submit(m_BlueShader, m_SquareVA);
            Renderer::Submit(m_Shader, m_VertexArray);

            Renderer::EndScene();

            m_CurrentWindow->OnUpdate();

            // Temporary
            m_CurrentWindow = m_WindowBottom.get();
            m_CurrentWindow->FrameDrawOn();

            m_CurrentWindow->OnUpdate();

            C3D_FrameEnd(0);
        }
    }
}