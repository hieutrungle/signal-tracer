# Ray Tracing for Wireless Communication

## Introduction

This project is a simulation of ray tracing for wireless communication. It is based on OpenGL and C++.

## Features

- [x] Ray tracing (multi-threading)
- [ ] Ray tracing (CUDA)
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
- [ ] spdlog: Logging for C++
- [ ] Boost.Asio: Network Programming for C++
- [ ] Dear ImGui: GUI for C++
- [ ] FreeType: Font for OpenGL
- [ ] irrKlang: Audio and Sound for C++
- [ ] OpenCV: Computer Vision for C++
- [ ] OpenAL: Audio and Sound for C++
- [ ] OpenCL: Parallel Computing for C++

## Installation

**OpenGL**: Install the latest drivers. We highly recommend the closed-source binary drivers. It’s not GNU or whatever, but they work. If your distribution doesn’t provide an automatic install, try Ubuntu’s guide.

Install all needed compilers, tools & libs. Complete list is : cmake make g++ libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxrandr-dev libxext-dev libxcursor-dev libxinerama-dev libxi-dev .

Use sudo apt-get install ***** or su && yum install ******.

**OpenCL**: sudo apt install opencl-headers ocl-icd-opencl-dev -y

**Boost**: the installation of Boost follows this guide: <https://linux.how2shout.com/how-to-install-boost-c-on-ubuntu-20-04-or-22-04/>

- Update Ubuntu Linux: `sudo apt update && sudo apt upgrade`
- Install the Developer’s tool: `sudo apt install build-essential g++ python3-dev autotools-dev libicu-dev libbz2-dev libboost-all-dev`
- Download the latest version of Boost from <https://www.boost.org/users/download/>
- extract the downloaded file: e.g. `tar xvf boost_1_80_0.tar.gz`
- Switch to the extracted directory: `cd boost_1_80_0`
- Setup Boost’s bootstrap, save the compiled library files under `/usr/include/boost`: `./bootstrap.sh --prefix=/usr/local`
- Install Boost: `sudo ./b2 install`

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
./build/bin/app.exe
```

Release run:

```bash
./build_release/bin/app.exe
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

### Coverage Map

![Direct Path](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/coverage/coverage_map_friss.png)

Signal strength increases as the color changes from red to cyan.

Simulation parameters:

- Frequency: 5.4 GHz
- Transmitter power: 10 dBm
- Transmitter antenna gain: 0 dBi
- Receiver antenna gain: 0 dBi

### Basic Demo

Execution time:

- Building BVH: 0.5 seconds
- Imaging method: 0.5 seconds
- Ray Casting method:

  - Signal Tracing, 20 reflections, 0.1 angle step:

    - Sequential: 1 minute 16 seconds
    - Parallel(12 cores): 11 seconds

#### Basic Direct Path

![Direct Path](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/basic_demo/direct.png)

#### Basic Reflect Path

- Single reflection:

![Reflect Path - 1 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/basic_demo/reflect1.png)

- Double reflection:

![Reflect Path - 2 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/basic_demo/reflect2.png)

### City Demo

Execution time:

- Building BVH: 57 seconds

#### Imaging Method

- Image Tracing: 50 seconds

##### City Direct Path

![Direct Path](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/imaging/direct.png)

##### City Reflect Path

- Single reflection:

![Reflect Path - 1 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/imaging/reflect1.png)

- Double reflection:

![Reflect Path - 2 reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/imaging/reflect2.png)

#### Ray Casting Method

- Ray Casting:

  - Signal Tracing, 20 reflections, 0.1 angle step:

    - Sequential: N/A
    - Parallel (12 cores): 28 seconds

##### Reflection

- Direct:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_direct.png)

- 1 Reflection:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_1_reflection.png)

- 2 Reflections:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_2_reflections.png)

- 3 Reflections:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_3_reflections.png)

- 4 Reflections:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_4_reflections.png)

- 6 Reflections:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_6_reflections.png)

- 7 Reflections:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_7_reflections.png)

- 19 Reflection:

![1 Reflection](https://github.com/hieutrungle/signal-tracer/blob/main/assets/images/city_demo/ray_casting/ray_casting_city_demo_19_reflections.png)

## In Progress

- [x] Coverage Map - Simple
- [ ] Coverage Map - EM
- [ ] Parallelize BVH Initialization (CUDA and multi-threading)
- [ ] Parallelize Ray Tracing (CUDA)
- [ ] Reflection coefficient using Fresnel equations (currently following Recommendation ITU-R P.2040-2, only works for 'TM' or 'TE' polarization)
- [ ] Refraction coefficient using Fresnel equations (currently following Recommendation ITU-R P.2040-2, only works for 'TM' or 'TE' polarization)
- [ ] Diffraction Ray Tracing

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