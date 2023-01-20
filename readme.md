# Voronoi Diagram

Render a dynamic Voronoi diagram on the GPU with C and OpenGL.

Also compiled to WASM + WebGL for the web. Check out the
[online demo](https://ziap.github.io/voronoi).

![](logo.png)

## Algorithm

The voronoi diagram is rendered by drawing a 3d cone for every seed. Everything
is batched into a single draw call so this runs extremely fast even on WebGL.

## Requirements

**Native build:**

- A C99 compiler
- GNU make
- GLFW
- GLEW

**Web build:**

- clang
- wasm-ld
- wasm-opt

## Usage

```bash
# For debugging with gdb, output debug/voronoi
make debug

# Optimized release build, output build/voronoi
make build

# WASM build, output voronoi.wasm
make web 
```

For native build, just run the executable.

For web build, host the website with any server.

```bash
python -m http.server 8080
```

## License

This project is licensed under the [MIT License](LICENSE).
