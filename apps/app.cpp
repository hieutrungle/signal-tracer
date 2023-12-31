

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "cl_utils.hpp"

#include "cyGL.h"
#include "cubesphere.hpp"
#include "lighting.hpp"
#include "line.hpp"
#include "model.hpp"
#include "program_container.hpp"
#include "coverage_map.hpp"
#include "imaging_tracer.hpp"
#include "ray_casting_tracer.hpp"
#include "receiver.hpp"
#include "transmitter.hpp"
#include "quad.hpp"
#include "scene.hpp"
#include "viewing.hpp"
#include "path_record.hpp"
#include "propagation_params.hpp"
#include "theoratical_model.hpp"
#include "utils.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"

#include <filesystem>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

#include "coverage_tracer.hpp"
#include "coverage_map.hpp"
#include "map_draw.hpp"


void process_terminal_inputs(int argc, char* argv [], SignalTracer::WindowParams& window_params, SignalTracer::PropagationParams& prop_params);

int main(int argc, char* argv []) {

    SignalTracer::WindowParams window_params{ 1280, 720, "CommRay", true };
    SignalTracer::PropagationParams prop_params{ 5.4e9f, 10.0f, 1.0f, 1.0f, "TM" };

    // Process terminal arguments
    // ----------------------------------
    process_terminal_inputs(argc, argv, window_params, prop_params);

    auto viewing_ptr{ std::make_shared<SignalTracer::Viewing>(SignalTracer::Camera{}) };

    /*
        ----------------------------------
        SCENE
        ----------------------------------
    */

    SignalTracer::Scene scene{ window_params, viewing_ptr };
    std::filesystem::path current_path{ std::filesystem::current_path() };

    // Create and compile our GLSL program from the shaders
    // ----------------------------------
    cy::GLSLProgram city_prog{};
    if (!city_prog.BuildFiles(
        std::filesystem::path{ (current_path / "shader/model.vert") }.c_str(),
        std::filesystem::path{ (current_path / "shader/model.frag") }.c_str())) {
        std::cout << "Cannot build shader program" << std::endl;
        return EXIT_FAILURE;
    }

    cy::GLSLProgram radio_prog{};
    if (!radio_prog.BuildFiles(
        std::filesystem::path{ (current_path / "shader/radio.vert") }.c_str(),
        std::filesystem::path{ (current_path / "shader/radio.frag") }.c_str())) {
        std::cout << "Cannot build shader program" << std::endl;
        return EXIT_FAILURE;
    }

    cy::GLSLProgram cm_prog{};
    if (!cm_prog.BuildFiles(
        std::filesystem::path{ (current_path / "shader/radio.vert") }.c_str(),
        std::filesystem::path{ (current_path / "shader/signal.frag") }.c_str())) {
        std::cout << "Cannot build shader program" << std::endl;
        return EXIT_FAILURE;
    }

    cy::GLSLProgram line_prog{};
    if (!line_prog.BuildFiles(
        std::filesystem::path{ (current_path / "shader/radio.vert") }.c_str(),
        std::filesystem::path{ (current_path / "shader/line.frag") }.c_str())) {
        std::cout << "Cannot build shader program" << std::endl;
        return EXIT_FAILURE;
    }

    /*
        ----------------------------------
        TRACING
        ----------------------------------
    */

    // Load models
    // ----------------------------------
    // std::filesystem::path city_path{ current_path / "assets/demo_layouts/Basic_Demo/OBJ/Basic_Demo_Layout_OBJ.obj" };
    // auto city_model_ptr1{ std::make_shared<SignalTracer::Model>(city_path.string()) };
    // // glm::vec3 tx_pos{ 5.0f, 1.5f, 5.0f };
    // glm::vec3 tx_pos{ 5.0f, 7.0f, 5.0f };
    // glm::vec3 rx_pos{ -13.0f, 3.0f, -0.5f };

    std::filesystem::path city_path{ current_path / "assets/demo_layouts/City_Demo/OBJ/City_Map_Layout_OBJ.obj" };
    auto city_model_ptr1{ std::make_shared<SignalTracer::Model>(city_path.string()) };
    glm::vec3 tx_pos{ 0.0f, 7.5f, 20.0f };
    glm::vec3 rx_pos{ -13.0f, 3.0f, -0.5f };

    SignalTracer::Transmitter tx0{ 0, tx_pos, prop_params.frequency, prop_params.tx_power, prop_params.tx_gain };
    SignalTracer::Receiver rx0{ 0, rx_pos, prop_params.rx_gain };

    std::vector<std::reference_wrapper<SignalTracer::Receiver>> receivers_ref{ std::ref(rx0) };

    // Signal tracer
    // ----------------------------------
    std::vector<std::shared_ptr<SignalTracer::Model>> model_ptrs{ city_model_ptr1 };
    std::vector<std::reference_wrapper<SignalTracer::Model>> models{ *city_model_ptr1 };

    // TODO: generate coverage map
    float cell_size{ 1.0f };
    int max_reflection_count{ 16 };
    int num_rays{ int(3e6) }; // a bit higher than FHD resolution (1920 x 1080)
    // int num_rays{ 50 };
    SignalTracer::CoverageTracer cov_tracer{ models, max_reflection_count, num_rays };

    SignalTracer::CoverageMap cm{};
    {
        std::vector<SignalTracer::PathRecord> tmp_ref_records{};
        std::vector<SignalTracer::PathRecord>* ref_records_ptr{ nullptr };
        // ref_records_ptr = &tmp_ref_records;
        cm = cov_tracer.generate({ tx0 }, cell_size, ref_records_ptr);
        // SignalTracer::CoverageMap cm{ cov_tracer.generate({ tx0 }, cell_size, &tmp_ref_records) };
        rx0.add_reflection_records(tx0.get_id(), tmp_ref_records);
    }

    SignalTracer::MapDrawing map_drawing{ cm.get_num_row(), cm.get_num_col(), cm.get_cells() };

    // int max_reflection_count{ 20 };
    // int num_rays{ int(12e6) };

    auto timer = Utils::Timer{};
    SignalTracer::RayCastingTracer sig_tracer{ models, max_reflection_count, num_rays };
    timer.execution_time();
    timer.reset();
    {
        std::vector<SignalTracer::PathRecord> ref_records{};
        sig_tracer.trace_rays(tx0.get_position(), rx0.get_position(), ref_records);
        rx0.add_reflection_records(tx0.get_id(), ref_records);
    }
    timer.execution_time();

    std::cout << "Number of reflections: " << rx0.get_reflection_records().at(tx0.get_id()).size() << std::endl;

    // Signal Propagation Calculation
    // ----------------------------------
    SignalTracer::PropagationModel prop_model{ prop_params };
    std::cout << prop_model << std::endl;
    {
        auto ref_records{ rx0.get_reflection_records().at(tx0.get_id()) };
        prop_model.calc_all_propagation_properties(ref_records);

        std::cout << "Total receiving power: " << prop_model.calc_total_receiving_power(ref_records) << " dB" << std::endl;

        rx0.add_reflection_records(tx0.get_id(), ref_records);
    }

    // int ref_counts[max_reflection_count + 2] = {};
    // for (const auto& [transmitter_id, ref_records] : rx0.get_reflection_records()) {
    //     std::cout << "Transmitter ID: " << transmitter_id << std::endl;
    //     for (const auto& ref_record : ref_records) {
    //         ++ref_counts[ref_record.get_reflection_count()];
    //         std::cout << ref_record << std::endl;
    //     }
    // }

    // for (int i = 0; i < max_reflection_count + 2; ++i) {
    //     std::cout << "Reflection count: " << i << "\t" << ref_counts[i] << std::endl;
    // }

    /*
        ----------------------------------
        VISUALIZATION
        ----------------------------------
    */

    // Lighting
    // ----------------------------------
    auto directional_light_ptr{ std::make_shared<SignalTracer::DirectionalLight>() };
    auto flash_light{ std::make_shared<SignalTracer::FlashLight>() };
    flash_light->init(city_prog, glm::mat4{ 1.0f });

    // set drawing for city
    // ----------------------------------
    std::vector<std::shared_ptr<SignalTracer::Drawable>> model_drawable_ptrs(model_ptrs.begin(), model_ptrs.end());
    model_drawable_ptrs.emplace_back(std::make_shared<SignalTracer::MapDrawing>(map_drawing));
    std::vector<glm::mat4> model_mats(model_ptrs.size(), glm::mat4{ 1.0 });

    auto shader_city_prog{ std::make_shared<SignalTracer::ShaderProgram>(
        "city",
        city_prog,
        std::vector<std::shared_ptr<SignalTracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        model_drawable_ptrs,
        model_mats) };

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
    // TODO: replace this with tx, rx class objects
    auto station_object_ptr{ std::make_shared<SignalTracer::Cubesphere>(0.25f, 3, true) };
    std::vector<std::shared_ptr<SignalTracer::Drawable>> station_objects{ };
    std::vector<glm::mat4> station_model_mats{};
    for (const auto& pos : { tx_pos }) {
        // for (const auto& pos : { tx_pos, rx_pos }) {
        station_objects.push_back(station_object_ptr);
        station_model_mats.push_back(glm::translate(glm::mat4{ 1.0f }, pos));
    }

    auto shader_radio_prog{ std::make_shared<SignalTracer::ShaderProgram>(
        "radio_object",
        radio_prog,
        std::vector<std::shared_ptr<SignalTracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        station_objects,
        station_model_mats) };

    // TODO: to here

    // draw coverage map
    // ----------------------------------
    std::vector<std::shared_ptr<SignalTracer::Drawable>> map_drawable_ptrs{ std::make_shared<SignalTracer::MapDrawing>(map_drawing) };
    std::vector<glm::mat4> map_model_mats{ glm::mat4{ 1.0f } };

    auto shader_map_prog{ std::make_shared<SignalTracer::ShaderProgram>(
        "map",
        cm_prog,
        std::vector<std::shared_ptr<SignalTracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        map_drawable_ptrs,
        map_model_mats) };

    // drawing reflections
    //     ----------------------------------
    std::vector<std::shared_ptr<SignalTracer::Drawable>> lines{};
    std::vector<glm::mat4> line_model_mats{};

    for (auto& receiver : receivers_ref) {
        for (const auto& [transmitter_id, ref_records] : receiver.get().get_reflection_records()) {
            for (const auto& ref_record : ref_records) {
                lines.push_back(std::make_shared<SignalTracer::Line>(ref_record.get_points(), ref_record.get_reflection_count()));
                line_model_mats.push_back(glm::mat4{ 1.0f });
            }
        }
    }

    auto shader_line_prog{ std::make_shared<SignalTracer::ShaderProgram>(
        "line",
        line_prog,
        std::vector<std::shared_ptr<SignalTracer::Light>>{directional_light_ptr},
        std::vector<glm::vec3>{glm::vec3{ 1.0f }},
        lines,
        line_model_mats) };
    // TODO: wrapper to here

    scene.add_program(shader_city_prog);
    scene.add_program(shader_radio_prog);
    scene.add_program(shader_line_prog);
    scene.add_program(shader_map_prog);

    scene.render();

    return EXIT_SUCCESS;
}

void process_terminal_inputs(int argc, char* argv [], SignalTracer::WindowParams& window_params, SignalTracer::PropagationParams& prop_params) {
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
            else if (arg == "--frequency") {
                prop_params.frequency = std::stof(argv[++i]);
            }
            else if (arg == "--tx_power") {
                prop_params.tx_power = std::stof(argv[++i]);
            }
            else if (arg == "--tx_gain") {
                prop_params.tx_gain = std::stof(argv[++i]);
            }
            else if (arg == "--rx_gain") {
                prop_params.rx_gain = std::stof(argv[++i]);
            }
            else if (arg == "--polarization") {
                prop_params.polarization = argv[++i];
            }
            else {
                std::cout << "Invalid argument: " << arg << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}