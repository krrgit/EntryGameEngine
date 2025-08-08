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
	void C2DPrepareLayer::OnUpdate(Timestep ts, uint16_t screenSide)
	{
		ET_PROFILE_FUNCTION();

		C2D_Prepare();
		//C2D_SetTintMode(C2D_TintMult);
	}
	void C2DPrepareLayer::OnEvent(Event& event)
	{
	}
}