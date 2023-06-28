#pragma once

#ifndef SCENE_HPP
#define SCENE_HPP

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "cyGL.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"

#include "cubesphere.h"
#include "lighting.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "signal_tracer.hpp"
#include "viewing.hpp"
#include "callback.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace signal_tracer {

    struct WindowParams {
        int width{};
        int height{};
        std::string title{};
        bool debug_flag{ false };
    };

    struct ShaderProgram {
        // A thin wrapper for cy::GLSLProgram
        std::string name;
        cy::GLSLProgram program;
        std::vector<std::shared_ptr<Light>> lights;
        glm::vec3 model_mat{ 1.0f };
        glm::vec3 view_mat{ 1.0f };
        glm::vec3 proj_mat{ 1.0f };
    };
    class Scene {
    public:
        Scene(WindowParams& window_params, std::shared_ptr<Viewing>& viewing_ptr)
            : m_width{ window_params.width }
            , m_height{ window_params.height }
            , m_title{ window_params.title }
            , m_debug{ window_params.debug_flag }
            , m_viewing_ptr{ viewing_ptr }
            , m_window_ptr{ init_window() }
            , m_callback{ m_window_ptr, m_viewing_ptr } {
            init();
        }

        // Copy constructor
        Scene(const Scene& other) = delete;

        // Move constructor
        Scene(Scene&& other) noexcept
            : m_width{ other.m_width }
            , m_height{ other.m_height }
            , m_title{ other.m_title }
            , m_debug{ other.m_debug }
            , m_viewing_ptr{ other.m_viewing_ptr }
            , m_window_ptr{ other.m_window_ptr }
            , m_programs{ std::move(other.m_programs) }
            , m_models{ std::move(other.m_models) }
            , m_lights{ std::move(other.m_lights) }
            , m_signal_tracer_ptr{ std::move(other.m_signal_tracer_ptr) }
            , m_delta_time{ other.m_delta_time }
            , m_callback{ std::move(other.m_callback) } {
            other.m_window_ptr = nullptr;
        }

        // Copy assignment
        Scene& operator=(const Scene& other) = delete;

        // Move assignment
        Scene& operator=(Scene&& other) noexcept {
            if (this != &other) {
                m_width = other.m_width;
                m_height = other.m_height;
                m_title = other.m_title;
                m_debug = other.m_debug;
                m_viewing_ptr = other.m_viewing_ptr;
                m_window_ptr = other.m_window_ptr;
                m_programs = std::move(other.m_programs);
                m_models = std::move(other.m_models);
                m_lights = std::move(other.m_lights);
                m_signal_tracer_ptr = std::move(other.m_signal_tracer_ptr);
                m_delta_time = other.m_delta_time;
                m_callback = std::move(other.m_callback);
                other.m_window_ptr = nullptr;
            }
            return *this;
        }

        ~Scene() {
            std::clog << "Scene terminated.\n";
            glfwDestroyWindow(m_window_ptr);
            glfwTerminate();
        }

        /*
            ---------------------------------
            WINDOW INITIALIZATION
            ---------------------------------
        */
        GLFWwindow* init_window() {
            if (!glfwInit()) {
                std::cout << "Failed to initialize GLFW" << std::endl;
                exit(EXIT_FAILURE);
            }

            default_window_hints();
            GLFWwindow* window_ptr = glfwCreateWindow(m_width, m_height, "CommRay", nullptr, nullptr);
            if (!window_ptr) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                exit(EXIT_FAILURE);
            }
            glfwSetWindowUserPointer(window_ptr, this);
            glfwMakeContextCurrent(window_ptr);

            // Initialize GLAD
            // ----------------------------------
            int version = gladLoadGL(glfwGetProcAddress);
            if (version == 0) {
                std::cerr << "Failed to initialize GLAD\n";
                glfwTerminate();
                exit(EXIT_FAILURE);
            }
            std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << '\n';
            return window_ptr;
        }

        void init() {

            default_gl_state();
            default_window_state();
            if (m_debug) { set_debug(); }
            set_viewport(m_width, m_height);
            set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
            set_window_title(m_title);

            m_callback.set_callback();
        }

        void set_debug() {
            GLint flags;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
            if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                CY_GL_REGISTER_DEBUG_CALLBACK;
                puts("Debugging enabled.");
            }
            else
                puts("Debugging disabled.");
        }

        void default_gl_state() {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        void default_window_hints() {
            glfwWindowHint(GLFW_SAMPLES, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
            if (m_debug)
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }

        void default_window_state() {
            glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            return;
        }

        void set_viewport(int width, int height) {
            glViewport(0, 0, width, height);
        }

        void set_clear_color(float r, float g, float b, float a) {
            glClearColor(r, g, b, a);
        }

        void clear() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void swap_buffers() {
            glfwSwapBuffers(m_window_ptr);
        }

        void poll_events() {
            glfwPollEvents();
        }

        bool should_close() {
            return glfwWindowShouldClose(m_window_ptr);
        }

        GLFWwindow* get_window() {
            return m_window_ptr;
        }

        int get_width() {
            return m_width;
        }

        int get_height() {
            return m_height;
        }

        float get_aspect_ratio() {
            return static_cast<float>(m_width) / static_cast<float>(m_height);
        }

        std::string get_title() {
            return m_title;
        }

        void set_window_title(const std::string& title) {
            m_title = title;
            glfwSetWindowTitle(m_window_ptr, m_title.c_str());
        }

        /*
            ---------------------------
            Keyboard
            ---------------------------
        */
        void move_camera_keys() {
            if (glfwGetKey(m_window_ptr, GLFW_KEY_E) == GLFW_PRESS)
                m_viewing_ptr->process_keyboard(CameraMovement::FORWARD, m_delta_time);
            if (glfwGetKey(m_window_ptr, GLFW_KEY_Q) == GLFW_PRESS)
                m_viewing_ptr->process_keyboard(CameraMovement::BACKWARD, m_delta_time);
            if (glfwGetKey(m_window_ptr, GLFW_KEY_A) == GLFW_PRESS)
                m_viewing_ptr->process_keyboard(CameraMovement::LEFT, m_delta_time);
            if (glfwGetKey(m_window_ptr, GLFW_KEY_D) == GLFW_PRESS)
                m_viewing_ptr->process_keyboard(CameraMovement::RIGHT, m_delta_time);
            if (glfwGetKey(m_window_ptr, GLFW_KEY_W) == GLFW_PRESS)
                m_viewing_ptr->process_keyboard(CameraMovement::UP, m_delta_time);
            if (glfwGetKey(m_window_ptr, GLFW_KEY_S) == GLFW_PRESS)
                m_viewing_ptr->process_keyboard(CameraMovement::DOWN, m_delta_time);
        }

        /*
            ---------------------------------
            SHADER PROGRAM
            ---------------------------------
        */
        void add_program(std::shared_ptr<ShaderProgram>& program) {
            m_programs.emplace_back(program);
        }

        void add_programs(std::vector<std::shared_ptr<ShaderProgram>>& programs) {
            m_programs.insert(m_programs.end(), programs.begin(), programs.end());
        }

        /*
            ---------------------------------
            MODEL
            ---------------------------------
        */
        void add_model(const std::shared_ptr<Model>& model) {
            m_models.emplace_back(model);
        }

        void add_models(const std::vector<std::shared_ptr<Model>>& models) {
            m_models.insert(m_models.end(), models.begin(), models.end());
        }

        /*
            ---------------------------------
            LIGHTING
            ---------------------------------
        */
        void add_light(std::shared_ptr<Light>& light) {
            m_lights.emplace_back(light);
        }

        void add_lights(std::vector<std::shared_ptr<Light>>& lights) {
            m_lights.insert(m_lights.end(), lights.begin(), lights.end());
        }

        /*
            ---------------------------------
            SIGNAL TRACER
            ---------------------------------
        */
        void set_signal_tracer(const std::shared_ptr<SignalTracer>& sig_tracer) {
            m_signal_tracer_ptr = sig_tracer;
        }

        /*
            ---------------------------------
            VIEWING
            ---------------------------------
        */
        void set_viewing(const std::shared_ptr<Viewing>& viewing) {
            m_viewing_ptr = viewing;
        }

        void set_delta_time(float delta_time) {
            m_delta_time = delta_time;
        }

        // float get_delta_time() {
        //     return m_delta_time;
        // }

        /*
            ---------------------------------
            RENDERING
            ---------------------------------
        */
        void render() {
            // TODO rendering

            // 
            // clear();
            // for (auto& model : m_models) {
            //     model->draw(m_programs, m_viewing_ptr, m_lights, m_signal_tracer_ptr);
            // }
            // swap_buffers();
            // poll_events();
        }

    private:
        int m_width{};
        int m_height{};
        std::string m_title{ "CommRay" };
        bool m_debug{ false };
        std::shared_ptr<Viewing> m_viewing_ptr{};
        GLFWwindow* m_window_ptr{};
        std::vector<std::shared_ptr<ShaderProgram>> m_programs{};
        std::vector<std::shared_ptr<Model>> m_models{};
        std::vector<std::shared_ptr<Light>> m_lights{};
        std::shared_ptr<SignalTracer> m_signal_tracer_ptr{};
        float m_delta_time{ 0.0f };
        GLFWCallback m_callback{};
    };
}


#endif // !SCENE_HPP