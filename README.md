# Entry Game Engine
 
A 3D game engine for the 3DS. The goal of this project is to lower the barrier to entry to 3DS game development. This project follows "The Cherno" game engine series on YouTube while trying to adapt it to the 3DS.

# Current Status
## Entrypoint
## Logging
Currently, logging is done by switching out the PrintConsole's framebuffer to a texture, and that texture is drawn to the screen via Citro2D. This part of the engine is not optimal, but allows users to render a scene and see the logs on the same screen.
## Event System
Polling & Event driven inputs supported.
## Application Layer
## Window Layer
## Layers
## ImGui
This ImGui implementation has been better optimized for the 3DS. Everything is drawn using the C2D library.
## Input Polling
## Button Codes
## Renderer
There exists some abstraction between the renderer and the Citro3D library. If there is no need to create an editor which runs on PC, or support other APIs, this abstraction may be removed for potentially better performance.
## Renderer API Abstraction
## Scripting Language
// TODO
## Entity Component System
// TODO
