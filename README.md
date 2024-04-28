# Entry Game Engine
 
A 3D game engine for the 3DS. The goal of this project is to simplify 3DS game development while remaining somewhat performant. This project follows "The Cherno" game engine series on YouTube while trying to adapt it to the 3DS.

## Status Demo 
// TODO

## Current Status
### Entrypoint
This is what's called when the application starts.
### Logging
Using spdlog for logging. Currently, logging is rendered by setting the default PrintConsole's framebuffer to a texture that is drawn to the screen via Citro2D. This part of the engine is not optimal (tanks fps in some cases), but allows users to render a scene and see the logs on the same screen. SELECT toggles the Logs display.
### Event System
Event driven inputs supported.
### Application Layer
Customizing which screen is 2D/3D, or only rendering one screen is a WIP. At the moment, both screens support 3D & 2D rendering.
### Window Layer
Each screen's render target is handled by the window. 
### Layers
These layers act as a way to customize what order to run parts of the game. Overlays are always rendered on top of 3D elements. 
### ImGui
This ImGui implementation has been better optimized for the 3DS. Everything is drawn using the C2D library. Does not support docking.
### Input Polling
For now, GetButtonDown() is implemented. IsKeyPressed() is mostly used atm. Need to Implement GetButton() and GetButtonUp().
### Button Codes
Same as Citro3D.
### Renderer

### Renderer API Abstraction
There exists some abstraction between the renderer and the Citro3D library. If there are no plans to implement an editor application on PC, or support other APIs, this abstraction may be removed for potentially better performance.
