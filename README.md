# Shader Playground

Create shaders easily !

Same use as https://shadertoy.com, but for desktop environment.

Created using C++ 17, OpenGL 4.6, GLFW 3.3.8, GLEW 2.2.0 for debug and GLEW 2.1.0 for release, and GLM 0.9.9.

IDE used : Visual Studio 2022, CMake project. Available on Windows, Linux and MacOS.


## How to use

All further paths are relative to the binary's folder (where the executable is).

When running the program inside Visual Studio 2022 or the executable, you have a terminal waiting for your input.<br>
It explains you everything you need to know.

1. Write a fragment shader (<name>.glsl) in a `res/shaders/` folder.
1. Run the application.
1. Enter the name of your fragment shader, without the path and without the extension.
	1. For example, if you created `res/shaders/fractals/mandelbrot.glsl`, write `mandelbrot` to the prompter.
1. Enjoy your shader.

Type "quit" or "exit" to terminate the program.

Some helpful commands while running :
- `Esc` : hide the window to return to the prompter, to load a new shader. You do not need to qui the application to load a newly created shader.
- `F5` : Hot-reload the shader that is currently running, without closing the window. It makes easy-to-develop.
- `F11` : Toggle fullscreen (windowed fullscreen borderless). It does not hide the taskbar of your OS.


### Development

Your fragment shaders are included in the main fragment shader code. So, you don't have to specify the `#version`. The version used is `460 core`. You also don't need to declare neither the main function and the in/out/uniform variables.<br>
Your fragment must be located in the `res/shaders/` folder.<br>
You can use the `#include <path/to/chunk>` directive to include other `.glsl` files, making it easy to factorize your code and make it more reusable. Do not specify the extension while including. It will search in the `res/shaders/` folder.
Your fragment shader file must contain a `vec3 mainImage()` function, which will be called by the `main()` fragment shader function at runtime.
You must assign a color to the predefined `vec4 FragColor` variable to set the pixel's color.

No circular dependencies can happen, because this directive is handled only in the `.frag` files, and only `.glsl` can be included.

**Note :** Remember to setup your project's paths correctly to GLFW, GLEW and GLM with the config.cmake file !


### Global variables

In your shaders, you can access various variables. Here's the list :

* `FragCoord`: a vec2 with current pixel coordinates, between 0 and the window's resolution.
* `MVP`: a mat4, the model-view-projection matrix.
* `M`: a mat4, the model matrix.
* `V`: a mat4, the view matrix.
* `P`: a mat4, the projection matrix.
* `ivMouse`: a vec2, the current mouse position in the window.
* `fvCenter`: a vec2, the center of the camera. See below for further explanations.
* `uvResolution`: a vec2, the size of the window, and thus of the surface to which the shader will render.
* `fTime`: a float, the total time elapsed, in seconds.
* `fDelta`: a float, the delta time that is the duration between the last frame and the current.
* `fRatio`: a float, the ratio of the window, thus the surface (width/height).
* `fZoom`: a float, the current level of zoom. See below for further explanations.


### The zoom and center uniforms

User can zoom pressing Shift, and unzoom pressing Control.<br>
It will zoom lerping to the mouse position (center of the window if the mouse is outside).

The `zoom` and `center` uniforms are updated consequently.


## More informations

- Zoom is not implemented yet.


## Author

NoxFly


## License

This project is under [the GPL-3.0 license](./LICENSE).
