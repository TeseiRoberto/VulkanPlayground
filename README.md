# Graphics Playground (GfxP)

My attempt at writing a graphics rendering library with support for multiple 
graphics API backends (only Vulkan right now).

This project serves for me as a playground to experiment with the desing
of a graphics library, renderer architectures, and to learn the Vulkan API.


## Project structure
The project is divided into two components:
- gfxp (Graphics Playground): an API agnostic graphics library
- gfxpTestBed: simple executable program which implements a render based on gfxp library


## Dependencies
The project has the following dependencies:
* GLFW: To handle window creation and user input

## How to build
The project uses CMake as a build system

## Notes
To develop the gfxp Vulkan backend I followed some tutorials such as:
* https://vulkan-tutorial.com/
* https://www.howtovulkan.com/
