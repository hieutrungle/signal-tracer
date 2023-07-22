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

<!-- // // TODO: Setup world that contains all the objects' triangles similar to the HittableList class in Ray Tracing in One Weekend. This HittableList is a wrapper of all meshes from all models. The BVHMap is a wrapper of all triangles from all meshes. The BVHMap is used to accelerate the intersection test between a ray and the triangles. The HittableList is used to find the closest intersection point between a ray and the triangles.

// // TODO: Setup a world.

/*
    HittableList model{};
    auto material = make_shared<Material>(attributes);
    model.add(make_shared<Triangle>(p1, p2, p3, material));

    ------------------------------------------
    BVH constructor
    BVH(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end) { init BVH structure }
    BVH(const HittableList& model) : BVH{ model.objects(), 0, model.objects().size() } {}

    There are two ways to initialize a BVH:
    1. Initialize a BVH with a HittableList object (a model).
    2. Initialize a BVH with a vector of shared_ptr<Hittable> objects (shared pointers of all triangles).

    we choose the first method. Initialize a model object with all triangles from all meshes. Then initialize a BVH with the model object.
    ------------------------------------------

    std::shared_ptr<BVH> bvh = std::make_shared<BVH>(model);

    ------------------------------------------
    There can be many model we want to render. We can store all models in a vector that is a member of the world object (a bigger HittableList).
    HittableList member variable:
    std::vector<shared_ptr<Hittable>> m_objects{};

    Child class of Hittable:
        class BVH : public
        class Triangle : public
        class HittableList : public
    ------------------------------------------

    HittableList model1{};
    auto material1 = make_shared<Material>(attributes1);
    model1.add(make_shared<Triangle>(p11, p12, p13, material1));

    HittableList model2{};
    auto material2 = make_shared<Material>(attributes2);
    model2.add(make_shared<Triangle>(p21, p22, p23, material2));

    std::shared_ptr<BVH> bvh1 = std::make_shared<BVH>(model1);
    std::shared_ptr<BVH> bvh2 = std::make_shared<BVH>(model2);

    std::vector<std::shared_ptr<Hittable>> models{};
    models.push_back(bvh1);
    models.push_back(bvh2);

    HittalbeList world{};
    world.add(models);

    world.intersect(ray, interval, record);

    ------------------------------------------
    When calling world.intersect(), the world object will call the intersect() function of each model object. The model object, which is a BVH, will perform the intersection test between the ray and and its AABB.
    If the ray intersects with the AABB, the BVH object will call the intersect() function of its left and right child nodes. The left and right child nodes are either BVH objects or Triangle objects (leaf nodes are triangles).
    If the traversal arrives at a leaf node (a triangle) of the BVH structure, that Triangle object will call its intersect() function to record the intersection information to the IntersectRecord object.
    ------------------------------------------
*/

/*
    // TODO: Implement reflective paths based on mirror reflection.
*/ -->