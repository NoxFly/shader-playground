# Copilot Instructions for Shader Playground

## Project Overview
- **Shader Playground** is a C++17 desktop application for writing, hot-reloading, and visualizing OpenGL fragment shaders, inspired by ShaderToy.
- Major components:
  - `src/` and `include/`: Core C++ logic (App, shader management, model loading, utilities)
  - `bin/res/shaders/`: User and example fragment shaders, plus reusable GLSL includes
  - `CMakeLists.txt` and `config.cmake`: Build configuration and external dependency paths

## Build & Run
- **Build system:** CMake (see `CMakeLists.txt`)
- **Configure dependencies:**
  - Copy `config-example.cmake` to `config.cmake` and set paths for GLEW, GLFW, and GLM
- **Build commands:**
  - Windows (Visual Studio): Open as CMake project or use CMake presets (`CMakePresets.json`)
  - CLI: `cmake -B build -S . && cmake --build build`
- **Run:**
  - Executable is placed in `bin/` (with subfolders for debug/release)
  - Run from terminal or via Visual Studio

## Shader Workflow
- Place user fragment shaders in `bin/res/shaders/` (or subfolders)
- Shaders can `#include <helpers/common>` or other `.glsl` files (no extension in include)
- Main entrypoint for user shaders: `void mainImage()`
- Do **not** declare `main()`, `#version`, or uniform/in/out variables in user shaders; these are injected by the app
- Example shader load: entering `fractals/mandelbrot` at the prompt loads `bin/res/shaders/fractals/mandelbrot.frag`

## Hot Reload & Controls
- `F5`: Hot-reload current shader
- `F8`: Toggle FPS limit
- `F9`: Reset runtime variables (zoom, camera, etc.)
- `F11`: Toggle fullscreen
- Arrow keys, Shift, Ctrl, Tab, and number keys control camera, zoom, and uniforms (see README for details)

## Project Patterns & Conventions
- **No circular includes** in shader `#include` directives; only `.glsl` files can be included
- All shader variables (uniforms, in/out) are managed by the app and injected at runtime
- Main C++ logic is in `App.cpp` (monolithic, not highly modularized)
- Utility functions for string manipulation in `utils.cpp`
- Model loading via `modelLoader.cpp`/`.hpp`
- Shader compilation and hot-reload logic in `shader.cpp`/`.hpp`

## External Dependencies
- GLEW, GLFW, GLM: Set up via `config.cmake` (see `config-example.cmake`)
- OpenGL 4.6 required

## Key Files & Directories
- `src/App.cpp`, `include/App.hpp`: Main application logic and event loop
- `src/shader.cpp`, `include/shader.hpp`: Shader loading, compilation, and hot-reload
- `bin/res/shaders/`: User and example shaders, plus reusable GLSL includes
- `README.md`: Detailed usage, controls, and shader authoring guide

---

For more details, see the project [README.md](../README.md).
