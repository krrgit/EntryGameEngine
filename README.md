# Entry Game Engine
 
A 3D game engine for the 3DS. The goal of this project is to simplify 3DS game development while remaining somewhat performant. This project follows "The Cherno" game engine series on YouTube while deviating to adapt it to the 3DS and use 3D instead of 2D.

## 3DS GPU Wiki (For reference)
https://www.3dbrew.org/wiki/Category:GPU

# Build Instructions for .3dsx

Install devkitPro (https://github.com/devkitPro/installer).
Run "make" in terminal within the same directory as the Makefile to build.

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
[x] Render Top Screen to ImGui.
[x] Create Layout + Windows.
[x] Implement OpenGL API.
[x] Build Standalone Windows program.
[ ] Play/Pause/Reset Scenes.
[ ] Render mesh wireframe in editor camera. 
[ ] Render editor grid in editor camera.
[ ] Render collider wireframes.
[ ] Options to build/clean build/clean game from editor.
[ ] Bottom screen scene editing into editor.
[ ] Render Bottom Screen to ImGui (need to figure out how to handle bottom screen.)

## Renderer
[x] Simple3D Renderer (has Camera, renders cubes, quads.)
[x] Implement .obj rendering support. (fastObjMesh)
[x] Implement Materials.
[x] Load .mtl files into materials.
[x] Support other model files (.fbx)
[ ] Implement Lighting.
[ ] Shaders for certain art styles (Toon, Unlit/Baked Lighting, Lit, Vertex Lit.)
[ ] "Bind" shaders to materials.
[ ] Implement Shadows. (Realtime(?), Projection, faded circle fake shadow thing)
[ ] Implement Effects (built-in to the GPU.)
[ ] Implement Animations.
[ ] (Refactor for Meshes) Simple Batch Rendering (works for quads/cubes only (useless.))
[ ] Implement Texture Generation.

## Game Systems (the big ones)
[x] Entity Component System (Custom ECS system)
[x] Scene Management System
[-] Native Scripting/C# Scripting (C# not implemented)
[ ] Physics Engine + Collision System/Components (Bullet? Newton Dynamics? PhysX? Jolt?)
[ ] Asset Management System
[ ] Audio
[ ] Networking (?)

## Quality of Life Things
[x] Use premake instead of make so it's easy to build.
[ ] (Editor) Drag-n-drop models into scene to create entity.
[ ] (Editor) Copy/Paste entities/components.
 
## Renderer API Abstraction
There exists some abstraction between the renderer and the Citro3D library. If there are no plans to implement an editor application on PC, or support other APIs, this abstraction may be removed for potentially better performance.

## Notes on Batch Rendering
It seems all objects are rendered at once, regardless of when DrawElements is called. So, if the engine calls DrawElements and replaces the data in the buffers, the overwritten data doesn't get drawn. So, I landed on the current implementation.