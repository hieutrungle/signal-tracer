#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "config.hpp"
#include "cyGL.h"
#include "cubesphere.h"
#include "lighting.hpp"
#include "line.hpp"
#include "model.hpp"
#include "orbit_camera.hpp"
#include "program_container.hpp"
#include "signal_tracer.hpp"
#include "scene.hpp"
#include "viewing.hpp"

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

    // auto city_model_ptr{ std::make_shared<signal_tracer::Model>("/home/hieule/research/comm_ray/assets/demo_layouts/City_Demo/OBJ/City_Map_Layout_OBJ.obj") };

    // Lighting
    // ----------------------------------
    auto directional_light_ptr{ std::make_shared<signal_tracer::DirectionalLight>() };
    auto flash_light{ std::make_shared<signal_tracer::FlashLight>() };
    flash_light->init(city_prog, glm::mat4{ 1.0f });

    // set drawing for city
    // ----------------------------------
    std::vector<std::shared_ptr<signal_tracer::Model>> model_ptrs{ city_model_ptr };
    std::vector<std::shared_ptr<signal_tracer::Drawable>> model_drawable_ptrs(model_ptrs.begin(), model_ptrs.end());
    std::vector<glm::mat4> model_mats{ };
    for (std::size_t i = 0; i < model_ptrs.size(); ++i) {
        model_mats.push_back(glm::mat4{ 1.0f });
    }

    // Signal tracer
    glm::vec3 tx_pos{ 5.0f, 2.5f, 5.0f };
    // glm::vec3 tx_pos{ 0.0f, 2.5f, 20.0f };
    glm::vec3 rx_pos{ -13.0f, 3.0f, -0.5f };

    Utils::Timer timer{};
    timer.reset();
    auto sig_tracer{ std::make_shared<signal_tracer::SignalTracer>(model_ptrs, 2) };
    timer.execution_time();
    timer.reset();
    sig_tracer->tracing(tx_pos, rx_pos);
    timer.execution_time();
    scene.set_signal_tracer(sig_tracer);

    auto shader_city_prog{ std::make_shared<signal_tracer::ShaderProgram>(
        "city",
        city_prog,
        std::vector<std::shared_ptr<signal_tracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        model_drawable_ptrs,
        model_mats) };
    scene.add_program(shader_city_prog);


    // TODO: Wrap this in a drawing tracer object
    // TODO: From here
    /*
        This object is used to draw both radio station and reflections
        Object input arguments:
        - tx_pos, rx_pos: positions of the transmitter and receiver
        - sig_tracer: a shared pointer to the signal tracer object


        Methods:
        set_program()
        set_light(light, program)
    */

    // set drawing for radio stations
    // ----------------------------------
    auto station_object_ptr{ std::make_shared<signal_tracer::Cubesphere>(0.25f, 3, true) };
    std::vector<std::shared_ptr<signal_tracer::Drawable>> station_objects{ };
    std::vector<glm::mat4> station_model_mats{};
    for (const auto& pos : { tx_pos, rx_pos }) {
        station_objects.push_back(station_object_ptr);
        station_model_mats.push_back(glm::translate(glm::mat4{ 1.0f }, pos));
    }

    auto shader_radio_prog{ std::make_shared<signal_tracer::ShaderProgram>(
        "radio_object",
        radio_prog,
        std::vector<std::shared_ptr<signal_tracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        station_objects,
        station_model_mats) };

    // drawing reflections
    //     ----------------------------------
    std::vector<std::shared_ptr<signal_tracer::Drawable>> lines{};
    std::vector<glm::mat4> line_model_mats{};

    for (const auto& ref_record : sig_tracer->get_reflection_records()) {
        lines.push_back(std::make_shared<signal_tracer::Line>(ref_record));
        line_model_mats.push_back(glm::mat4{ 1.0f });
    }
    auto shader_line_prog{ std::make_shared<signal_tracer::ShaderProgram>(
        "line",
        line_prog,
        std::vector<std::shared_ptr<signal_tracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        lines,
        line_model_mats) };
    // TODO: wrapper to here

    scene.add_program(shader_radio_prog);
    scene.add_program(shader_line_prog);

    scene.render();

    return 0;
}

