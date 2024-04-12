#include "etpch.h"
#include <3ds.h>
#include <citro3d.h>
#include <string.h>
#include "vshader_shbin.h"
#include "Buffer.h"
#include "VertexArray.h"

using namespace Entry;

#define CLEAR_COLOR 0x191919FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projection;

std::shared_ptr<VertexArray> m_VertexArray;
std::shared_ptr<VertexArray> m_SquareVA;

static void sceneInit(void)
{
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[1]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	
	// Compute the projection matrix
	Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, 0.0, 1.0, true);

	m_VertexArray.reset(VertexArray::Create());

	float vertices[3 * 7] =
	{
		200.0f, 180.0f, 0.4f, 0.8f, 0.8f, 0.2f, 1.0f,
		100.0f,  60.0f, 0.4f, 0.8f, 0.2f, 0.8f, 1.0f,
		300.0f,  60.0f, 0.4f, 0.2f, 0.2f, 0.8f, 1.0f
	};

	std::shared_ptr<VertexBuffer> vertexBuffer;
	vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
	BufferLayout layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" },
	};

	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);
	

	u16 indices[3] = { 0, 1, 2 };
	std::shared_ptr<IndexBuffer> indexBuffer;
	indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint16_t)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA.reset(VertexArray::Create());
	float squareVertices[4 * 7] =
	{
		350.0f,  210.0f,  0.5f,  0.2f, 0.2f, 0.8f, 1.0f,
		 50.0f,  210.0f,  0.5f,  0.2f, 0.2f, 0.8f, 1.0f,
		 50.0f,  30.0f,   0.5f,  0.2f, 0.2f, 0.8f, 1.0f,
		350.0f,  30.0f,   0.5f,  0.2f, 0.2f, 0.8f, 1.0f,
	};

	std::shared_ptr<VertexBuffer> squareVB;
	squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	BufferLayout squareVBLayout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" },
	};
	squareVB->SetLayout(squareVBLayout);
	m_SquareVA->AddVertexBuffer(squareVB);

	u16 squareIndices[6] = { 0, 1, 2, 2, 3, 0};
	std::shared_ptr<IndexBuffer> squareIB;
	squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint16_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	// Configure the first fragment shading substage to just pass through the vertex color
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
}

static void sceneRender(void)
{
	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);

	// Draw the VBO
	// C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
	m_SquareVA->Bind();
	C3D_DrawElements(GPU_GEOMETRY_PRIM, m_SquareVA->GetIndexBuffer()->GetCount(), C3D_UNSIGNED_SHORT, m_SquareVA->GetIndexBuffer()->GetDataPointer());
	
	m_VertexArray->Bind();
	C3D_DrawElements(GPU_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), C3D_UNSIGNED_SHORT, m_VertexArray->GetIndexBuffer()->GetDataPointer());
}

static void sceneExit(void)
{
	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}

int main()
{
	// Initialize graphics
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Initialize debug console
	consoleInit(GFX_BOTTOM, NULL);
	printf("\nconsole:\n");

	// Initialize the render target
	C3D_RenderTarget* target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Initialize the scene
	sceneInit();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C3D_RenderTargetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
			C3D_FrameDrawOn(target);
			sceneRender();
		C3D_FrameEnd(0);
	}

	// Deinitialize the scene
	sceneExit();

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();
	return 0;
}
