#pragma once

#include <glm/glm.hpp>

namespace Entry {
	class Camera {
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {} // TODO: FIX right side

		const glm::mat4& GetProjection(uint16_t screenSide = 0) const { return m_Projection; }

		// TODO:
		// SetPerspective, SetOrthographic, m_ProjectionRightSide (Stereo 3D)
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}