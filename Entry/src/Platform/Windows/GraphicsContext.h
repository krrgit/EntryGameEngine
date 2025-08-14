/* Each window has a graphics context.
 * This abstraction is for the graphics library.
 * This allows for each window to have different graphics libraries (OpenGL, Vulkan, DirectX, etc.)
 * Since this doesn't exist for the 3DS (only citro3D), this abstraction was removed for 3DS builds.
*/
#pragma once

namespace Entry {
	class GraphicsContext {
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	private:

	};
}
