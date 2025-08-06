# Entry Game Engine
 
A 3D game engine for the 3DS. The goal of this project is to simplify 3DS game development while remaining somewhat performant. This project follows "The Cherno" game engine series on YouTube while deviating to adapt it to the 3DS and use 3D instead of 2D.

## 3DS GPU Wiki (For reference)
https://www.3dbrew.org/wiki/Category:GPU

# How to Build

Entry Game Engine uses Make at the moment. Run "make" in the terminal to build.

# Getting Started

The engine works in two parts: the engine (/Entry), and the game (/Sandox). Sandbox is where all your game assets should go. 
SandboxApp.cpp is where your game code starts. 
1. Create a custom layer which inherits from the Layer class. 
1. Add your code to its members (OnAttach, OnUpdate, OnDetach, OnImGuiRender, OnEvent.)
1. Push your layer onto the stack in SandboxApp.cpp in the constructor, and pass which screen you want to render it on (ET_WINDOW_TOP or ET_WINDOW_BOTTOM). i.e:
```
Sandbox()
{
	PushLayer(new Sandbox3D(), ET_WINDOW_TOP);
}
```
4. Build and run.
1. Voila!

## Status Demo 
// TODO
- Update ImGui implementation to docking branch.
- 3D Model loading and rendering.

## Current Status
### Entrypoint
This is what's called when the application starts.
### Logging
Using spdlog for logging. Currently, logging is rendered by setting the default PrintConsole's framebuffer to a texture that is drawn to the screen via Citro2D. ~~This part of the engine is not optimal (tanks fps in some cases)~~ (It now performs much better, ~0.5ms to redraw when log is updated (update: but still buggy when printing too fast)), but allows users to render a scene and see the logs on the same screen. SELECT toggles the Logs display.
### Event System
Event driven inputs supported.
### Application Layer
Customizing which screen is 2D/3D, or only rendering one screen is a WIP. At the moment, both screens support 3D & 2D rendering.
### Window Layer
Each screen's render target is handled by the window. 
### Layers & Overlays
These layers act as a way to customize what order to run parts of the game. Overlays are always rendered on top of 3D elements. 
### ImGui
This ImGui implementation has been better optimized for the 3DS. Everything is drawn using the C2D library. Does not support docking (yet.)
### Input Polling
For now, GetButtonDown() is implemented. IsKeyPressed() is mostly used atm. Need to Implement GetButton() and GetButtonUp().
### Button Codes
Same as Citro3D.
### Renderer
Position, Rotation, Scale supported.
Textures supported.

### Renderer API Abstraction
There exists some abstraction between the renderer and the Citro3D library. If there are no plans to implement an editor application on PC, or support other APIs, this abstraction may be removed for potentially better performance.

## My notes on citro3d works (probably wrong)
### 3 Texture Units (not 4)
citro3d allows 3 texture unit in a C3D_Context. But, according to the specs of the PICA200, there are 4. I don't know how to access the last one.
### TexEnv
There are 6 TexEnvs. They work like a pipeline, to create various effects.
The output of one TexEnv is fed to the next TexEnv.
These are setup with C3D_TexEnvSrc and C3D_TexEnvFunc.
To render without effects, only TexEnv 0 is needed.
### Batch Rendering (WIP)
Anytime C3D_DrawElements (this is what draws objects on screen, given a vertex buffer linked in BufInfo), the texture(s) it uses is determined by the TexEnv 0. 
The textures cannot be selected by the vertex shader. 
So, (to me) it seems impossible to batch multiple objects with different textures.
You can update the TexEnv to switch texture sources.
However, the vertex shader must accompany this by setting the appropriate ".out outtc0 texcoord0", ".out outtc1 texcoord1", or ".out outtc2 texcoord2". 
At the moment, I set them all to the same values (gains > loss in performance probably?).
At the moment,vertices are batched by texture into separate vertex buffers. Then, each buffer does 1 a draw call.
It's only useful for objects with the same texture (i.e. buildings, trees, foliage, etc.)