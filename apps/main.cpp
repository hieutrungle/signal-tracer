#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "config.hpp"


#if defined(CAMERA)
#include "camera.hpp"
#elif defined(FPS_VIEW)
#include "fps_camera.hpp"
#elif defined(ORBIT_VIEW)
#include "orbit_camera.hpp"
#endif

#include "utils.hpp"
#include "model.hpp"
#include "scene_config.hpp"
#include "lighting.hpp"
#include "cyGL.h"
#include "Cubesphere.h"
#include "signal_tracer.hpp"
#include "ray.hpp"
#include "line.hpp"
#include "point_container.hpp"
#include "callback.hpp"
#include "triangle_draw.hpp"
#include "viewing.hpp"
#include "triangle.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <vector>
#include <memory>


// camera
#if defined(ORBIT_CAMERA_HPP)
std::shared_ptr<OrbitCamera> camera{std::make_shared<OrbitCamera>(glm::vec3(0.0f, 40.0f, 30.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))};
#endif

int WIDTH = 1280, HEIGHT = 720;
Viewing viewing{ camera, WIDTH, HEIGHT };

int main(int argc, char* argv []) {

    // process terminal arguments
    // ----------------------------------
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg{ argv[i] };
            if (arg == "-w" || arg == "--width") {
                WIDTH = std::stoi(argv[++i]);
            }
            else if (arg == "-h" || arg == "--height") {
                HEIGHT = std::stoi(argv[++i]);
            }
        }
    }

    float aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    SceneConfig::default_window_hints();
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CommRay", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    // ----------------------------------
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << '\n';

    // configure global opengl state
    // -----------------------------
    SceneConfig::default_gl_state();
    SceneConfig::default_window_state(window);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    // Calback
    // ----------------------------------
    // Set the required callback functions
    glfwSetKeyCallback(window, GLFWCallback::keyCallback);
    glfwSetFramebufferSizeCallback(window, GLFWCallback::framebuffer_size_callback);

#ifdef FPS_VIEW
    glfwSetCursorPosCallback(window, GLFWCallback::mouse_fps_callback);
    glfwSetScrollCallback(window, GLFWCallback::scroll_fps_callback);
#elif defined(ORBIT_VIEW)
    glfwSetMouseButtonCallback(window, GLFWCallback::mouse_button_callback);
    glfwSetCursorPosCallback(window, GLFWCallback::mouse_cursor_callback);
    glfwSetScrollCallback(window, GLFWCallback::scroll_callback);
#endif

    // Debugging
#ifdef DEBUG
    SceneConfig::set_debug();
#endif

    // Create and compile our GLSL program from the shaders
    // ----------------------------------
    cy::GLSLProgram city_prog{};
    if (city_prog.BuildFiles("/home/hieule/research/comm_ray/shader/model_loading.vert", "/home/hieule/research/comm_ray/shader/model_loading.frag")) {}

    cy::GLSLProgram line_prog{};
    if (line_prog.BuildFiles("/home/hieule/research/comm_ray/shader/line.vert", "/home/hieule/research/comm_ray/shader/line.frag")) {}

    cy::GLSLProgram radio_prog{};
    if (radio_prog.BuildFiles("/home/hieule/research/comm_ray/shader/sphere.vert", "/home/hieule/research/comm_ray/shader/sphere.frag")) {}

    // Load models
    // ----------------------------------
    Model city_model{ "/home/hieule/research/comm_ray/assets/demo_layouts/Basic_Demo/OBJ/Basic_Demo_Layout_OBJ.obj" };
    // Model city_model{ "/home/hieule/research/comm_ray/assets/demo_layouts/City_Demo/OBJ/City_Map_Layout_OBJ.obj" };

    // Signal tracer
    // ----------------------------------
    Utils::Timer timer{}; // TODO:
    timer.reset();
    signal_tracer::SignalTracer sig_tracer{ city_model.get_meshes(), 1 };
    timer.execution_time();

    // TX-RX positions
    // ----------------------------------
    radio_prog.Bind();
    Cubesphere radio_object{ 0.5f, 3, true };
    std::vector<glm::vec3> radio_positions {
        glm::vec3(-10.0f, 3.0f, 5.0f),
            glm::vec3(-13.0f, 3.0f, -0.5f)
    };
    // std::vector<glm::vec3> radio_positions {
    //     glm::vec3(1.0f, 3.0f, 0.0f),
    //         glm::vec3(13.0f, 3.0f, -0.5f)
    // };
    glm::vec3 tx_pos {radio_positions[0]};
    glm::vec3 rx_pos {radio_positions[1]};

    timer.reset();
    sig_tracer.tracing(radio_positions[0], radio_positions[1]);
    timer.execution_time();

    for (const auto& ref_record : sig_tracer.get_reflection_records()) {
        std::cout << "reflection record: " << ref_record << std::endl;
    }

    // Lighting
    // ----------------------------------
    Lighting::DirectionalLight directional_light(
        glm::vec3(0.0f, -1.0f, 0.0f), // direction
        glm::vec3(1.0f, 1.0f, 1.0f), // color
        0.2, // ambient
        0.5, // diffuse
        0.5, // specular
        "directional_light"
    );

    Lighting::FlashLight flash_light(
        glm::vec3(0.0f, 0.0f, 0.0f), // position
        glm::vec3(0.0f, 0.0f, -1.0f), // direction
        glm::vec3(1.0f, 1.0f, 1.0f), // color
        0.1, // ambient
        0.5, // diffuse
        0.5, // specular
        1.0f, // constant
        0.09f, // linear
        0.032f, // quadratic
        glm::cos(glm::radians(10.0f)), // inner cut off
        glm::cos(glm::radians(30.0f)), // outer cut off
        "flash_light"
    );
    flash_light.init(city_prog, glm::mat4{ 1.0f });

    // Line
    // ----------------------------------
    line_prog.Bind();
    std::vector<signal_tracer::Line> lines{};
    // transform algorithm std
    std::transform(sig_tracer.get_reflection_records().begin(), sig_tracer.get_reflection_records().end(), std::back_inserter(lines), [](const signal_tracer::ReflectionRecord& ref_record) {
        return signal_tracer::Line{ ref_record };
        });

    // Triangles
    // signal_tracer::Triangle triangle_1{ glm::vec3(10.827774, 4.497894, -0.824997), glm::vec3(9.050000, 4.497894, -0.824997), glm::vec3(9.050000, 2.720123, -0.825000) };
    // signal_tracer::PointContainer point_container1{std::vector<glm::vec3>{triangle_1[0], triangle_1[1], triangle_1[2]}};
    // signal_tracer::TriangleDrawing triangle_drawing1{point_container1};

    // signal_tracer::PointContainer point_container2{std::vector<glm::vec3>{glm::vec3(14.950012, 5.020142, -16.425003), glm::vec3(-15.050049, 5.020142, -16.425003), glm::vec3(-15.050049, 1.020142, -16.425003)}};
    // signal_tracer::TriangleDrawing triangle_drawing2{point_container2};

    // delta time
    // ----------------------------------
    float accu_delta_time{};
    int frame_count{ 0 };
    float fps_last_time{ 0.0f };
    float last_frame_time{ 0.0f };

    // render loop
    // ----------------------------------
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {

        // FPS counter
        // -------------------------
        accu_delta_time += ((float) glfwGetTime() - fps_last_time);
        fps_last_time = static_cast<float>(glfwGetTime());
        frame_count++;
        if (accu_delta_time >= 1.0) {
            std::string fps = "FPS: " + std::to_string(frame_count);
            std::string ms = Utils::round_to_string(1000.0 / (float) frame_count, 2) + "ms";
            std::string title = "CommRay | " + fps + " | " + ms;
            glfwSetWindowTitle(window, title.c_str());
            frame_count = 0;
            accu_delta_time = 0.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view{ 1.0f };
        glm::mat4 projection{ 1.0f };
        glm::mat4 model{ 1.0f };
        glm::mat4 normal_matrix{ 1.0f };

        // Camera
        // -------------------------
        view = viewing.get_view_matrix();
        projection = glm::perspective(glm::radians(viewing.get_camera_fov()), aspect_ratio, 0.1f, 100.0f);

        // -------------------------
        directional_light.color(Lighting::WHITE);
        directional_light.init(city_prog, view);

        city_prog.Bind();
        model = glm::mat4{ 1.0f };
        normal_matrix = glm::transpose(glm::inverse(view * model));
        city_prog.SetUniformMatrix4("view", glm::value_ptr(view), 1, false);
        city_prog.SetUniformMatrix4("projection", glm::value_ptr(projection), 1, false);
        city_prog.SetUniformMatrix4("model", glm::value_ptr(model), 1, false);
        city_prog.SetUniformMatrix4("view_model", glm::value_ptr(view * model), 1, false);
        city_prog.SetUniformMatrix4("normal_matrix", glm::value_ptr(normal_matrix), 1, false);

        city_model.draw(city_prog);
        // city_model.draw(city_prog);

        // Draw radio_object 1
        // -------------------------
        directional_light.color(Lighting::RED);
        directional_light.init(radio_prog, view);
        model = glm::mat4{ 1.0f };
        model = glm::translate(model, tx_pos);
        model = glm::scale(model, glm::vec3(0.25f));
        radio_object.draw(radio_prog, model, view, projection);

        // Draw radio_object 2
        // -------------------------
        directional_light.color(Lighting::GREEN);
        directional_light.init(radio_prog, view);
        model = glm::mat4{ 1.0f };
        model = glm::translate(model, rx_pos);
        model = glm::scale(model, glm::vec3(0.25f));
        radio_object.draw(radio_prog, model, view, projection);

        // directional_light.color(Lighting::BLUE);
        // directional_light.init(radio_prog, view);
        // model = glm::mat4{ 1.0f };
        // model = glm::translate(model, mirror_point_tx);
        // model = glm::scale(model, glm::vec3(0.25f));
        // radio_object.draw(radio_prog, model, view, projection);

        // Draw line
        // -------------------------
        line_prog.Bind();
        directional_light.color(Lighting::BLACK);
        directional_light.init(line_prog, view);
        for (auto& line : lines) {
            if (line.get_reflection_count() == viewing.get_draw_reflection_mode()) {
                model = glm::mat4{ 1.0f };
                line.draw(line_prog, model, view, projection);
            }
        }

        // model = glm::mat4{ 1.0f };
        // tmp_line.draw(line_prog, model, view, projection);

        // Draw triangles
        // -------------------------
        // model = glm::mat4{ 1.0f };
        // triangle_drawing1.draw(line_prog, model, view, projection);
        // triangle_drawing2.draw(line_prog, model, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Camera
        // -------------------------
        viewing.set_delta_time(static_cast<float>(glfwGetTime()) - last_frame_time);
        GLFWCallback::move_camera_keys(window, static_cast<float>(glfwGetTime()) - last_frame_time);
        last_frame_time = (float) glfwGetTime();
    }

    glfwTerminate();
    return 0;
}

