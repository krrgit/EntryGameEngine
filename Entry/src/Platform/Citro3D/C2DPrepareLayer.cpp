#include "etpch.h"
#include "C2DPrepareLayer.h"
#include <citro2d.h>

namespace Entry {
	void C2DPrepareLayer::OnAttach()
	{
	}
	void C2DPrepareLayer::OnDetach()
	{
	}
	void C2DPrepareLayer::OnUpdate(Timestep ts)
	{
		C2D_Prepare();
	}
	void C2DPrepareLayer::OnEvent(Event& event)
	{
	}
}