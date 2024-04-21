#pragma once

#include <citro3d.h>

namespace Entry {
	class Shader {
	public:
		Shader(int src_id);
		~Shader();

		void Bind();
		void Unbind() const;

		void UploadUniformMat4(std::string name, const C3D_Mtx* matrix);
	private:
		DVLB_s* vshader_dvlb;
		shaderProgram_s program;
	};
}