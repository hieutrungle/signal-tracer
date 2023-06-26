# Ray Tracing for Wireless Communication

## Introduction

This project is a simulation of ray tracing for wireless communication. It is based on OpenGL and C++.

## Features

- [x] Ray tracing
- [x] Reflection
- [ ] Refraction
- [ ] Diffraction
- [ ] Scattering
- [x] Texture
- [ ] GUI
- [x] 3D Model
- [ ] 3D Animation
- [x] 3D Interaction
- [ ] 3D Sound
- [ ] 3D Video
- [ ] 3D VR
- [ ] 3D AR

## Dependencies

- [x] OpenGL
- [x] C++
- [x] CMake
- [x] GLFW
- [x] GLAD
- [x] GLM
- [x] Assimp
- [x] stb_image
- [ ] Dear ImGui: GUI for C++
- [ ] FreeType: Font for OpenGL
- [ ] irrKlang: Audio and Sound for C++
- [ ] OpenCV: Computer Vision for C++
- [ ] OpenAL: Audio and Sound for C++
- [ ] OpenCL: Parallel Computing for C++

## Installation

Install the latest drivers. We highly recommend the closed-source binary drivers. It’s not GNU or whatever, but they work. If your distribution doesn’t provide an automatic install, try Ubuntu’s guide.

Install all needed compilers, tools & libs. Complete list is : cmake make g++ libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxrandr-dev libxext-dev libxcursor-dev libxinerama-dev libxi-dev .

Use sudo apt-get install ***** or su && yum install ******.

## Usage

### Build

Debug build:

```bash
export PARALLEL_CORES=$(($(grep -c "^processor" /proc/cpuinfo) - 1))
export PARALLEL_CORES=$(($(nproc) - 1))
echo "Building with $PARALLEL_CORES cores"

mkdir -p ./build
cd ./build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j$PARALLEL_CORES
```

Release build:

```bash
export PARALLEL_CORES=$(($(grep -c "^processor" /proc/cpuinfo) - 1))
export PARALLEL_CORES=$(($(nproc) - 1))
echo "Building with $PARALLEL_CORES cores"

mkdir -p ./build
cd ./build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$PARALLEL_CORES
```

### Run

Debug run:

```bash
./build/bin/main.exe
```

Release run:

```bash
./build_release/bin/main.exe
```

### Mouse

- Left click: Pan
- Right click: Arcball rotation
- Middle click: Move forward/backward
- Scroll: Zoom in/out - change field of view

### Keyboard

- W: Move up
- S: Move down
- A: Move left
- D: Move right
- Q: Move backward
- E: Move forward

## Results

### Basic Demo

#### Direct Path

![Direct Path](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/direct.png)

#### Reflect Path

**Single reflection:**

![Reflect Path - 1 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/reflect1.png)

**Double reflection:**

![Reflect Path - 2 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/reflect2.png)

### City Demo

#### Direct Path

![Direct Path](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/direct.png)

#### Reflect Path

**Single reflection:**

![Reflect Path - 1 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/reflect1.png)

**Double reflection:**

![Reflect Path - 2 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/reflect2.png)

## Tips

Those are equivalent to the following commands:

```cpp
shader_program["objectColor"].Set(0.0f, greenValue, 0.0f, 1.0f);
```

equal to

```cpp
int vertexColorLocation{};
shader_program.RegisterUniform(vertexColorLocation, "objectColor");
shader_program.SetUniform("objectColor", 0.0f, greenValue, 0.0f, 1.0f);
```

equal to

```cpp
int vertexColorLocation = glGetUniformLocation(shader_program.GetID(), "objectColor");
glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
```

- Texture

```cpp
glUniform1i(glGetUniformLocation(shader_program, "texture0"), 0);
```

equal to

```cpp
shader_program.SetUniform("texture0", 0);
```
