#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "config.hpp"
#include "orbit_camera.hpp"

#include "utils.hpp"
#include "model.hpp"
#include "scene_config.hpp"
#include "lighting.hpp"
#include "cyGL.h"
#include "cubesphere.h"
#include "signal_tracer.hpp"
#include "ray.hpp"
#include "line.hpp"
#include "point_container.hpp"
#include "callback.hpp"
#include "triangle_draw.hpp"
#include "viewing.hpp"
#include "triangle.hpp"
#include "scene.hpp"
#include "program_container.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <vector>
#include <memory>


int main(int argc, char* argv []) {

    signal_tracer::WindowParams window_params{ 1280, 720, "CommRay", true };

    // Process terminal arguments
    // ----------------------------------
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg{ argv[i] };
            if (arg == "-w" || arg == "--width") {
                window_params.width = std::stoi(argv[++i]);
            }
            else if (arg == "-h" || arg == "--height") {
                window_params.height = std::stoi(argv[++i]);
            }
            else if (arg == "-t" || arg == "--title") {
                window_params.title = argv[++i];
            }
            else if (arg == "-d" || arg == "--debug") {
                window_params.debug_flag = true;
            }
        }
    }

    std::shared_ptr<OrbitCamera> camera{ std::make_shared<OrbitCamera>(glm::vec3(0.0f, 40.0f, 30.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) };
    std::shared_ptr<Viewing> viewing_ptr{ std::make_shared<Viewing>(camera) };

    /*
        ----------------------------------
        |            Scene               |
        ----------------------------------
    */
    signal_tracer::Scene scene{ window_params, viewing_ptr };

    // Create and compile our GLSL program from the shaders
    // ----------------------------------
    cy::GLSLProgram city_prog{};
    if (city_prog.BuildFiles("/home/hieule/research/comm_ray/shader/model.vert", "/home/hieule/research/comm_ray/shader/model.frag")) {}

    cy::GLSLProgram line_prog{};
    if (line_prog.BuildFiles("/home/hieule/research/comm_ray/shader/line.vert", "/home/hieule/research/comm_ray/shader/line.frag")) {}

    cy::GLSLProgram radio_prog{};
    if (radio_prog.BuildFiles("/home/hieule/research/comm_ray/shader/sphere.vert", "/home/hieule/research/comm_ray/shader/sphere.frag")) {}


    // Load models
    // ----------------------------------
    auto city_model_ptr{ std::make_shared<signal_tracer::Model>("/home/hieule/research/comm_ray/assets/demo_layouts/Basic_Demo/OBJ/Basic_Demo_Layout_OBJ.obj") };

    auto city_model_ptr2{ std::make_shared<signal_tracer::Model>("/home/hieule/research/comm_ray/assets/demo_layouts/Basic_Demo/OBJ/Basic_Demo_Layout_OBJ.obj") };
    city_model_ptr2->transform(glm::translate(glm::vec3{ 0.0f, 0.0f, -15.0f }));

    // Model city_model_ptr{ "/home/hieule/research/comm_ray/assets/demo_layouts/City_Demo/OBJ/City_Map_Layout_OBJ.obj" };


    // Lighting
    // ----------------------------------
    auto directional_light{ std::make_shared<signal_tracer::DirectionalLight>(
        glm::vec3(0.0f, -1.0f, 0.0f), // direction
        glm::vec3(1.0f, 1.0f, 1.0f), // color
        0.2, // ambient
        0.5, // diffuse
        0.5, // specular
        "directional_light"
    ) };

    auto flash_light{ std::make_shared<signal_tracer::FlashLight>(
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
    ) };
    flash_light->init(city_prog, glm::mat4{ 1.0f });

    // Signal tracer
    // ----------------------------------
    Utils::Timer timer{};
    timer.reset();
    std::vector<signal_tracer::Model> models{ *city_model_ptr, *city_model_ptr2 };
    auto sig_tracer{ std::make_shared<signal_tracer::SignalTracer>(models, 2) };
    timer.execution_time();

    timer.reset();
    sig_tracer->tracing(glm::vec3(-10.0f, 3.0f, 5.0f), glm::vec3(-13.0f, 3.0f, -0.5f));
    timer.execution_time();

    sig_tracer->init_draw();

    scene.set_signal_tracer(sig_tracer);

    // Shader Programs
    // ----------------------------------
    auto shader_city_prog{ std::make_shared<signal_tracer::ShaderProgram>(
        "city",
        city_prog,
        std::vector<std::shared_ptr<signal_tracer::Light>>{directional_light},
        std::vector<std::shared_ptr<signal_tracer::Drawable>>{city_model_ptr, city_model_ptr2}) };

    // auto shader_radio_prog{ std::make_shared<signal_tracer::ShaderProgram>(
    // "radio_object",
    // radio_prog,
    // std::vector<std::shared_ptr<signal_tracer::Light>>{directional_light},
    // std::vector<std::shared_ptr<signal_tracer::Drawable>>{city_model_ptr, city_model_ptr2}) };

    scene.add_program(shader_city_prog);

    // delta time
    // ----------------------------------
    float accu_delta_time{};
    int frame_count{ 0 };
    float fps_last_time{ 0.0f };
    float last_frame_time{ 0.0f };

    // render loop
    // ----------------------------------
    while (!scene.should_close()) {

        // FPS counter
        // -------------------------
        accu_delta_time += ((float) glfwGetTime() - fps_last_time);
        fps_last_time = static_cast<float>(glfwGetTime());
        frame_count++;
        if (accu_delta_time >= 1.0) {
            std::string fps = "FPS: " + std::to_string(frame_count);
            std::string ms = Utils::round_to_string(1000.0 / (float) frame_count, 2) + "ms";
            window_params.title = "CommRay | " + fps + " | " + ms;
            scene.set_window_title(window_params.title);
            frame_count = 0;
            accu_delta_time = 0.0;
        }

        scene.clear();

        glm::mat4 view{ 1.0f };
        glm::mat4 projection{ 1.0f };
        glm::mat4 model{ 1.0f };

        // Camera
        // -------------------------
        view = viewing_ptr->get_view_matrix();
        projection = glm::perspective(glm::radians(viewing_ptr->get_camera_fov()), scene.get_aspect_ratio(), 0.1f, 100.0f);

        // Draw city model
        // -------------------------
        // directional_light->set_color(Constant::WHITE);
        // directional_light->init(city_prog, view);
        // city_model_ptr->draw(city_prog, glm::mat4{ 1.0f }, view, projection);
        // city_model_ptr2->draw(city_prog, glm::mat4{ 1.0f }, view, projection);
        scene.render_step();

        // TODO: put in shader programs
        // TODO: Tracer only does the tracing, not the drawing
        // TODO: Put the drawing in shader programs
        // Draw radio objects
        // -------------------------
        directional_light->set_color(Constant::WHITE);
        directional_light->init(radio_prog, view);
        sig_tracer->draw_radio_stations(radio_prog, glm::mat4{ 1.0f }, view, projection);

        // Draw lines
        // -------------------------
        directional_light->set_color(Constant::BLACK);
        directional_light->init(line_prog, view);
        sig_tracer->set_display_reflection_count(viewing_ptr->get_draw_reflection_mode());
        sig_tracer->draw(line_prog, glm::mat4{ 1.0f }, view, projection);

        scene.swap_buffers();
        scene.poll_events();

        // Camera
        // -------------------------
        scene.set_delta_time(static_cast<float>(glfwGetTime()) - last_frame_time);
        scene.move_camera_keys();
        last_frame_time = (float) glfwGetTime();
    }

    return 0;
}
