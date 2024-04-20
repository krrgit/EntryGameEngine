#include "etpch.h"
#include "Application.h"

#include "Input.h"

#include "vshader00_shbin.h"

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

        // Load the vertex shader, create a shader program and bind it
        vshader_dvlb = DVLB_ParseFile((u32*)vshader00_shbin, vshader00_shbin_size);
        shaderProgramInit(&program);
        shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
        C3D_BindProgram(&program);

        m_Shader.reset(new Shader(0));

        // Get the location of the uniforms
        uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");

        // Configure attributes for use with the vertex shader
        AttrInfo_Init(&m_AttrInfo);
        AttrInfo_AddLoader(&m_AttrInfo, 0, GPU_FLOAT, 3); // v0=position
        AttrInfo_AddLoader(&m_AttrInfo, 1, GPU_FLOAT, 4); // v0=position

        // Compute the projection matrix
        Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, 0.0, 1.0, true);

        float vertices[] = {
            200.0f, 200.0f, 0.5f,0.8f, 0.8f, 0.2f, 1.0f,
            100.0f, 40.0f, 0.5f, 0.8f, 0.2f, 0.8f, 1.0f,
            300.0f, 40.0f, 0.5f, 0.2f, 0.2f, 0.8f, 1.0f,
        };

        // Create the VBO (vertex buffer object)
        vbo_data = linearAlloc(sizeof(vertices));
        memcpy(vbo_data, vertices, sizeof(vertices));

        // Configure buffers
        BufInfo_Init(&m_BufInfo);
        BufInfo_Add(&m_BufInfo, vbo_data, sizeof(float)*7, 2, 0x10);

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

            //C3D_BindProgram(&program);
            m_Shader->Bind();
            C3D_SetAttrInfo(&m_AttrInfo);
            C3D_SetBufInfo(&m_BufInfo);
            // Update the uniforms
            C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);

            // Draw the VBO
            C3D_DrawArrays(GPU_TRIANGLES, 0, 3);

            C2D_Prepare();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            C2D_Flush();

            m_Window->OnUpdate();

            m_Window->FrameEnd();
        }
    }
}