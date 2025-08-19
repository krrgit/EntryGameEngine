#include"etpch.h"
#include "Citro3DRendererAPI.h"

#include<citro3d.h>
#include"Citro3DWindow.h"
#include"Entry/Core/Application.h"

namespace Entry {
	void Citro3DRendererAPI::Init()
	{
		ET_PROFILE_FUNCTION();
		gfxInitDefault();
		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

		// Configure the first fragment shading substage to just pass through the vertex color
		// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
	}

	Citro3DRendererAPI::~Citro3DRendererAPI()
	{
		romfsExit();
		gfxExit();
	}


	void Citro3DRendererAPI::SetClearColor(const uint32_t color)
	{
		static_cast<Citro3DWindow*>(&Application::Get().GetWindow())->SetClearColor(color);
	}

	void Citro3DRendererAPI::Clear()
	{
		//C3D_RenderTargetClear(m_RenderTarget, C3D_CLEAR_ALL, m_ClearColor, 0);
	}

	void Citro3DRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint16_t indexCount)
	{
		ET_PROFILE_FUNCTION();

		uint16_t count = indexCount > 0 ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		C3D_DrawElements(
			GPU_TRIANGLES,
			count,
			C3D_UNSIGNED_SHORT,
			vertexArray->GetIndexBuffer()->GetDataPointer()
		);
	}


}
