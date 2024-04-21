#include "etpch.h"
#include "Application.h"

#include "Input.h"

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
    {
        int logScreen = SCREEN_TOP;
        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
        ET_CORE_ASSERT(logScreen > 1 && logScreen < -1, "Improper screen selected for console.");

        if (logScreen == SCREEN_NULL) 
        {
            WindowProps topProps("Top", 400, 240, SCREEN_TOP);
            std::unique_ptr<Window>(Window::Create(topProps));

            WindowProps bottomProps("Bottom", 320, 240, SCREEN_BOTTOM);
            m_Window = std::unique_ptr<Window>(Window::Create(bottomProps));
            m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        } else {
            Entry::Log::Init(logScreen);
            
            ET_CORE_WARN("Initialized Log!");
            ET_INFO("Hello!");

            uint8_t otherScreen = logScreen ^ 1;
            int width = otherScreen == SCREEN_TOP ? 400 : 320;
            std::string name = otherScreen == SCREEN_TOP ? "Top" : "Bottom";

            WindowProps props(name, width, 240, otherScreen);
            m_Window = std::unique_ptr<Window>(Window::Create(props));
            m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
        }

        m_Shader.reset(new Shader(0));

        // Get the location of the uniforms
        uLoc_projection = m_Shader->GetUniformLocation("projection");


        // Compute the projection matrix
        Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, 0.0, 1.0, true);

        m_VertexArray.reset(VertexArray::Create());
        
        float vertices[] = {
            200.0f, 200.0f, 0.5f,0.8f, 0.8f, 0.2f, 1.0f,
            100.0f, 40.0f, 0.5f, 0.8f, 0.2f, 0.8f, 1.0f,
            300.0f, 40.0f, 0.5f, 0.2f, 0.2f, 0.8f, 1.0f,
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
            350.0f,  210.0f,  0.5f,
             50.0f,  210.0f,  0.5f,
             50.0f,  30.0f,   0.5f,
            350.0f,  30.0f,   0.5f,
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

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
        
    }

    void Application::Run()
    {
        


        while (aptMainLoop() && m_Running) {
            m_Window->FrameBegin();

            // Update the uniforms
            C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);

            m_BlueShader->Bind();
            m_SquareVA->Bind();
            C3D_DrawElements(GPU_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), C3D_UNSIGNED_SHORT, m_SquareVA->GetIndexBuffer()->GetDataPointer());
            
            m_Shader->Bind();
            m_VertexArray->Bind();
            C3D_DrawElements(GPU_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), C3D_UNSIGNED_SHORT, m_VertexArray->GetIndexBuffer()->GetDataPointer());


            C2D_Prepare();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            C2D_Flush();

            m_Window->OnUpdate();

            m_Window->FrameEnd();
        }
    }
}