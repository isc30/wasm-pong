# WebAssembly Pong

This is a test project to show the capabilities of WASM + WebGL 2.0

## Demo!

The demo can be found in: http://isc30.github.io/wasm-pong/demo/

## Features

The project consists on multiple parts glued together:

### Emscripten

* C++14 compiler to WebAssembly

* Virtual filesystem
  * On-demand resource loading over the network
  * File bundler (RAM)

### WebGL 2.0

* 3D renderer (hardware accelerated)
* Wrapper for OpenGL ES 3.0

### SDL2

* 2D renderer (software)
* IO (input, filesystem, etc)
* Window (context) management
* Resource loading (images, audio, etc)

### Custom (own)

* Step-based GameLoop
  * Compatible with single-threaded concurrent environments (WASM)
  * Precise DeltaTime between Ticks

* SDL2 wrapper for modern C++
  * Smart pointers
    * Difficult to cause memory leaks
    * Automatic memory/ownership management
  * OOP wrappers
  * Event queue

* Debugging tools
  * Profiles
  * Native compilation target (visual studio) for easy debugging
  
* OpenGL wrapper for modern C++
  * Smart pointers
  * Binding guards (scope based bind/unbind using RAII)
  * GLType to native type conversions
  * Automatic GLError checking
  * Context information (GPU, GL extensions, vendor, etc)
  
> Some pieces were taken from [ImasiEngine](https://bitbucket.org/imasi/imasiengine/src/master/ImasiEngine) (my old 3D engine project)

# How to Compile

## Visual Studio
There is a Visual Studio 2017 solution inside `project/vs2017`. Use the target `Debug/x64`.

## Manually

Use any GNU-based terminal (cmder is recommended in windows) and run `make wasm`.

The output files will appear inside the folder `build\wasm`.
