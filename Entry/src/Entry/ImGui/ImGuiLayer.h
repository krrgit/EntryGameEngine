#pragma once

#include "Entry/Layer.h"

namespace Entry {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:
		C3D_Tex* tex;
		Tex3DS_SubTexture m_subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
		C3D_RenderTarget* m_RenderTarget;
		std::vector<uint32_t> m_PixelBuffer;
		time_t m_Time;
		C2D_Image m_Image;
		uint16_t m_Width, m_Height;

	};
}