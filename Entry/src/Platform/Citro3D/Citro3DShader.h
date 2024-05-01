#pragma once

#include "Entry/Renderer/Shader.h"

#include <citro3d.h>
#include <glm/glm.hpp>

namespace Entry {
	class Citro3DShader : public Shader {
	public:
		Citro3DShader(u32* shbinData, u32 shBinSize);
		~Citro3DShader();

		virtual void Bind();
		virtual void Unbind() const;

		void UploadUniformInt(std::string name, int value);
		void UploadUniformFloat(std::string name, const float value);
		void UploadUniformFloat2(std::string name, const glm::vec2& value);
		void UploadUniformFloat3(std::string name, const glm::vec3& value);
		void UploadUniformFloat4(std::string name, const glm::vec4& value);

		void UploadUniformMat3(std::string name, const glm::mat3* matrix);
		void UploadUniformMat4(std::string name, const C3D_Mtx* matrix);
	private:
		uint32_t m_RendererID;
		DVLB_s* vshader_dvlb;
		shaderProgram_s program;
	};
}