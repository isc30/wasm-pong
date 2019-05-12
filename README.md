# WebAssembly Pong

This is a demo project to show the capabilities of WASM + WebGL 2.0

It consists on multiple parts glued together:

### SDL2
* 2D renderer (software)
* IO (input, filesystem, etc)
* Window (context) management
* Resource loading (images, audio, etc)

### WebGL 2.0
* 3D renderer (hardware accelerated)
* Wrapper for OpenGL ES 3.0

### Emscripten
* C++14 compiler to WebAssembly
* 
* Virtual filesystem
  * On-demand resource loading over the network
  * File bundler (RAM)

# How to Compile

## Visual Studio
There is a Visual Studio 2017 solution inside `project/vs2017`. Use the target `Debug/x64`.

## Manually

Use any GNU-based terminal (cmder is recommended in windows) and run `make wasm`.

The output files will appear inside the folder `build\wasm`.
