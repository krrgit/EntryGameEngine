#pragma once

#ifdef ET_PLATFORM_WINDOWS

#include <glm/glm.hpp>

namespace Entry
{
	namespace MathUtils
	{
		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	}

}
#endif // ET_PLATFORM_WINDOWS