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
	}

	void Citro3DRendererAPI::SetClearColor(const uint32_t color)
{
	static_cast<Citro3DWindow*>(&Application::Get().GetWindow())->SetClearColor(color);
}

void Citro3DRendererAPI::Clear()
{
	//C3D_RenderTargetClear(m_RenderTarget, C3D_CLEAR_ALL, m_ClearColor, 0);
}

void Citro3DRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
{
	C3D_DrawElements(
		GPU_TRIANGLES, 
		vertexArray->GetIndexBuffer()->GetCount(), 
		C3D_UNSIGNED_SHORT, 
		vertexArray->GetIndexBuffer()->GetDataPointer()
	);
}

}
