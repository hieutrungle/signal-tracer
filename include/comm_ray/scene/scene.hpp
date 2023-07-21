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

#include "utils.hpp"
#include "viewing.hpp"
#include "callback.hpp"
#include "program_container.hpp"
#include "radio_station.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace SignalTracer {

    /// @brief Handle the initilization of OpenGL, GLFW, and drawing
    class Scene {
    public:

        /// @brief Initialization with window parameters and a viewing camera
        /// @param window_params all necessary parameters for window initilization
        /// @param viewing_ptr encapsulates a camera object, 
        Scene(WindowParams& window_params, std::shared_ptr<Viewing>& viewing_ptr)
            : m_window_params{ window_params }
            , m_viewing_ptr{ viewing_ptr }
            , m_window_ptr{ init_window() }
            , m_callback{ m_window_ptr, m_viewing_ptr } {
            init();
        }

        // Copy constructor
        Scene(const Scene& other) = delete;

        // Move constructor
        Scene(Scene&& other) noexcept
            : m_window_params{ other.m_window_params }
            , m_viewing_ptr{ other.m_viewing_ptr }
            , m_window_ptr{ other.m_window_ptr }
            , m_callback{ std::move(other.m_callback) }
            , m_delta_time{ other.m_delta_time }
            , m_prog_ptrs{ std::move(other.m_prog_ptrs) } {
            other.m_window_ptr = nullptr;
        }

        // Copy assignment
        Scene& operator=(const Scene& other) = delete;

        // Move assignment
        Scene& operator=(Scene&& other) noexcept {
            if (this == &other) {
                return *this;
            }
            m_window_params = other.m_window_params;
            m_viewing_ptr = other.m_viewing_ptr;
            m_window_ptr = other.m_window_ptr;
            m_callback = std::move(other.m_callback);
            m_delta_time = other.m_delta_time;
            m_prog_ptrs = std::move(other.m_prog_ptrs);
            other.m_window_ptr = nullptr;
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
            GLFWwindow* window_ptr = glfwCreateWindow(m_window_params.width, m_window_params.height, "CommRay", nullptr, nullptr);
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
            if (m_window_params.debug_flag) { set_debug(); }
            set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
            set_window_title(m_window_params.title);

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
            if (m_window_params.debug_flag)
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }

        void default_window_state() {
            glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            return;
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
            return m_window_params.width;
        }

        int get_height() {
            return m_window_params.height;
        }

        float get_aspect_ratio() {
            return static_cast<float>(m_window_params.width) / static_cast<float>(m_window_params.height);
        }

        std::string get_title() {
            return m_window_params.title;
        }

        void set_window_title(const std::string& title) {
            m_window_params.title = title;
            glfwSetWindowTitle(m_window_ptr, m_window_params.title.c_str());
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
            m_prog_ptrs.emplace_back(program);
        }

        void add_programs(std::vector<std::shared_ptr<ShaderProgram>>& programs) {
            m_prog_ptrs.insert(m_prog_ptrs.end(), programs.begin(), programs.end());
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
            // delta time
            // ----------------------------------
            float accu_delta_time{};
            int frame_count{ 0 };
            float fps_last_time{ 0.0f };
            float last_frame_time{ 0.0f };
            while (!should_close()) {

                // FPS counter
                // -------------------------
                accu_delta_time += ((float) glfwGetTime() - fps_last_time);
                fps_last_time = static_cast<float>(glfwGetTime());
                frame_count++;
                if (accu_delta_time >= 1.0) {
                    std::string fps = "FPS: " + std::to_string(frame_count);
                    std::string ms = Utils::round_to_string(1000.0 / (float) frame_count, 2) + "ms";
                    m_window_params.title = "CommRay | " + fps + " | " + ms;
                    set_window_title(m_window_params.title);
                    frame_count = 0;
                    accu_delta_time = 0.0;
                }

                clear();

                render_step();

                swap_buffers();
                poll_events();

                set_delta_time(static_cast<float>(glfwGetTime()) - last_frame_time);
                move_camera_keys();
                last_frame_time = (float) glfwGetTime();
            }
        }

        void render_step() {
            glm::mat4 view_mat = m_viewing_ptr->get_view_matrix();
            glm::mat4 projection_mat = glm::perspective(glm::radians(m_viewing_ptr->get_camera_fov()), get_aspect_ratio(), 1.0f, 100.0f);

            // Draw models from shader programs
            // ----------------------------------
            for (auto& prog_ptr : m_prog_ptrs) {
                int light_idx{ 0 };
                for (const auto& light_ptr : prog_ptr->lights) {
                    light_ptr->set_color(prog_ptr->light_colors[light_idx]);
                    light_ptr->init(prog_ptr->program, view_mat);
                    light_idx++;
                }

                int drawable_idx{ 0 };
                for (const auto& drawable_ptr : prog_ptr->drawables) {
                    if (prog_ptr->name == "line") {
                        int display_ref_count = static_cast<Line*>(drawable_ptr.get())->get_reflection_count();
                        if (display_ref_count == m_viewing_ptr->get_draw_reflection_mode()) {
                            LineColor color = static_cast<LineColor>(display_ref_count);
                            prog_ptr->program.SetUniform("color", get_station_color(color) * 1.5f);
                            drawable_ptr->set_model_mat(prog_ptr->drawable_model_mats[drawable_idx]);
                            drawable_ptr->draw(prog_ptr->program, drawable_ptr->get_model_mat(), view_mat, projection_mat);
                        }
                    }
                    else {
                        if (prog_ptr->name == "radio_object") {
                            StationColor color = static_cast<StationColor>(drawable_idx);
                            prog_ptr->program.SetUniform("color", get_station_color(color));
                        }
                        drawable_ptr->set_model_mat(prog_ptr->drawable_model_mats[drawable_idx]);
                        drawable_ptr->draw(prog_ptr->program, drawable_ptr->get_model_mat(), view_mat, projection_mat);
                        drawable_idx++;
                    }
                }
            }
        }

    private:
        // Set up OpenGL and GLFW
        WindowParams m_window_params{};
        std::shared_ptr<Viewing> m_viewing_ptr{};
        GLFWwindow* m_window_ptr{};
        GLFWCallback m_callback{};
        float m_delta_time{ 0.0f };

        // Shader programs
        std::vector<std::shared_ptr<ShaderProgram>> m_prog_ptrs{};

    };
}


#endif // !SCENE_HPP