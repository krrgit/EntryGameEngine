#pragma once
#include<glm/glm.hpp>

#ifdef ET_PLATFORM_3DS
	#include <3ds.h>
	#include<citro3d.h>
#endif

namespace Entry {
	class PerspectiveCamera {
	public:

#ifdef ET_PLATFORM_3DS
		inline void glm_mat4_to_C3D_mtx(C3D_Mtx* out, glm::mat4& in)
		{
			Mtx_Zeros(out);

			glm::mat4 m = glm::transpose(in);

			for (int i = 0; i < 4; ++i) {
				out->r[i].x = m[i][0];
				out->r[i].y = m[i][1];
				out->r[i].z = m[i][2];
				out->r[i].w = m[i][3];
			}
		}
#endif

		PerspectiveCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3 GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix();
			//PrintViewMatrix();
		//PrintPosition();
		}
		
		const glm::vec4 GetRotation() const { return m_Rotation; }
		void SetRotation(const glm::vec4& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		void Set3DValue(float slider3DState) { m_Slider3DState = slider3DState; RecalculateProjectionViewMatrix(); }

		 
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix(uint16_t screenSide = 0) const { return screenSide == 0 ? m_ViewProjectionMatrix : m_ViewProjectionMatrixR ; }
	private:
		void RecalculateViewMatrix();
		void RecalculateProjectionViewMatrix();

#ifdef ET_PLATOFRM_3DS
		void PrintPosition() {
			consoleClear();
			printf("R: %.1f %.1f %.1f\n",
				m_Position.x, m_Position.y, m_Position.z);
		}

		void PrintViewMatrix() {
			consoleClear();

			glm::mat4 glm_in = m_ProjectionMatrix;


			/*printf("%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n",
				m_ViewProjectionMatrix[0].x, m_ViewProjectionMatrix[0].y, m_ViewProjectionMatrix[0].z, m_ViewProjectionMatrix[0].w,
				m_ViewProjectionMatrix[1].x, m_ViewProjectionMatrix[1].y, m_ViewProjectionMatrix[1].z, m_ViewProjectionMatrix[1].w,
				m_ViewProjectionMatrix[2].x, m_ViewProjectionMatrix[2].y, m_ViewProjectionMatrix[2].z, m_ViewProjectionMatrix[2].w,
				m_ViewProjectionMatrix[3].x, m_ViewProjectionMatrix[3].y, m_ViewProjectionMatrix[3].z, m_ViewProjectionMatrix[3].w
			);*/
			
			printf("%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n",
				glm_in[0].x, glm_in[0].y, glm_in[0].z, glm_in[0].w,
				glm_in[1].x, glm_in[1].y, glm_in[1].z, glm_in[1].w,
				glm_in[2].x, glm_in[2].y, glm_in[2].z, glm_in[2].w,
				glm_in[3].x, glm_in[3].y, glm_in[3].z, glm_in[3].w
			);

			C3D_Mtx test_mtx;

			glm_mat4_to_C3D_mtx(&test_mtx, glm_in);
			printf("==============\n");
			printf("%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n",
				test_mtx.r[0].c[0], test_mtx.r[0].c[1], test_mtx.r[0].c[2], test_mtx.r[0].c[3],
				test_mtx.r[1].c[0], test_mtx.r[1].c[1], test_mtx.r[1].c[2], test_mtx.r[1].c[3],
				test_mtx.r[2].c[0], test_mtx.r[2].c[1], test_mtx.r[2].c[2], test_mtx.r[2].c[3],
				test_mtx.r[3].c[0], test_mtx.r[3].c[1], test_mtx.r[3].c[2], test_mtx.r[3].c[3]
			);
		}
#endif // ET_PLATOFRM_3DS

	public:
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	private:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

		glm::mat4 m_ProjectionMatrixR = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrixR = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrixR = glm::mat4(1.0f);

		glm::vec3 m_Position = {0.0f, 0.0f, -1.0f};
		glm::vec4 m_Rotation = glm::vec4(0.0f);

		bool m_Is3DCamera;
		float m_Slider3DState = 0.0f;
	};
}