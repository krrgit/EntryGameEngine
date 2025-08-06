# Entry Game Engine
 
A 3D game engine for the 3DS. The goal of this project is to simplify 3DS game development while remaining somewhat performant. This project follows "The Cherno" game engine series on YouTube while deviating to adapt it to the 3DS and use 3D instead of 2D.

## 3DS GPU Wiki (For reference)
https://www.3dbrew.org/wiki/Category:GPU

# Build Instructions for .3dsx

Entry Game Engine uses Make, for now. Run "make" in the terminal to build.

# Getting Started

The engine works in two parts: the engine "/Entry", and the game "/Sandox". Sandbox is where all your game assets should go. 
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
4. Build and run (in Citra Emulator or on console).
1. Voila!

# Features and Status
## PC Editor
[x] Port ImGui to work in-engine.
[ ] Render screen(s) to ImGui.
[ ] Create Layout + Windows.
[ ] Implement OpenGL API.
[ ] Build Standalone Windows program.
[ ] Feature to compile game from editor.

## Renderer
[x] Simple3D Renderer (has Camera, renders cubes, quads.)
[x] Implement .obj rendering support. (fastObjMesh)
[x] Simple Batch Rendering (atm batch by texture, create multiple batches for same texture if needed. Maybe batch by materal in the future?)
[x] Instancing.
[ ] Implement Materials.
[ ] Load .mtl files into materials.
[ ] Implement Phong Lighting.
[ ] Implement Effects (built-in to the GPU.)
[ ] Support other model files (.fbx)
[ ] Implement Texture Generation.
[ ] Implement Shadows (?)
[ ] Implement Animations.

## Game Systems (the big ones)
[ ] Entity Component System (Flecs? needs C++11 support)
[ ] Scene Management System
[ ] Physics Engine + Collision System/Components (Bullet? Newton Dynamics? PhysX? Jolt?)
[ ] Scripting
[ ] Audio
[ ] Networking (?)

## Quality of Life Things
[ ] Use premake instead of make so it's easy to build.
 
## Renderer API Abstraction
There exists some abstraction between the renderer and the Citro3D library. If there are no plans to implement an editor application on PC, or support other APIs, this abstraction may be removed for potentially better performance.

## Notes on Batch Rendering
It seems all objects are rendered at once, regardless of when DrawElements is called. So, if the engine calls DrawElements and replaces the data in the buffers, the overwritten data doesn't get drawn. So, I landed on the current implementation.