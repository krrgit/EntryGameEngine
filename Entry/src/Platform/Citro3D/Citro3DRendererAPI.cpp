#include"etpch.h"
#include "Citro3DRendererAPI.h"

#include<citro3d.h>
#include"Citro3DWindow.h"
#include"Entry/Application.h"

namespace Entry {

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
