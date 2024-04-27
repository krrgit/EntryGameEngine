#pragma once

#include "Entry/Layer.h"
#include "Core.h"


namespace Entry {
	class C2DPrepareLayer: public Layer
	{
	public:
		C2DPrepareLayer() {}
		~C2DPrepareLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& event) override;
	};
}