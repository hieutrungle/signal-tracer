#pragma once 
#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include "base_camera.hpp"
#include "constant.hpp"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "utils.hpp"
#include <cmath>
#include <vector>

namespace SignalTracer {
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    namespace CameraDefaultParam {
        inline glm::vec3 position{ 0.0f, 40.0f, 30.0f };
        inline glm::vec3 target{ 0.0f, 0.0f, 0.0f };
        inline glm::vec3 world_up{ 0.0f, 1.0f, 0.0f };
        inline float fov{ 45.0 };
    }

    class Camera {
    public:

        Camera(
            glm::vec3 position = CameraDefaultParam::position,
            glm::vec3 target = CameraDefaultParam::target,
            glm::vec3 world_up = CameraDefaultParam::world_up,
            float fov = CameraDefaultParam::fov
        );

        glm::mat4 get_view_matrix() const;
        float get_fov() const;
        void set_fov(float fov);

        void reset();

        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void process_keyboard(CameraMovement direction, float velocity);

        void pan(float xoffset, float yoffset, GLboolean constrain_pitch = true);
        void arcball_rotate(float xoffset, float yoffset);
        void zoom(float yoffset);

        friend std::ostream& operator<<(std::ostream& os, const Camera& base_camera) {
            os << "camera info: " << std::endl;
            os << "position: " << glm::to_string(base_camera.m_position) << std::endl;
            os << "target: " << glm::to_string(base_camera.m_target) << std::endl;
            os << "radius: " << glm::length(base_camera.m_position - base_camera.m_target) << std::endl;
            os << "front: " << glm::to_string(base_camera.m_cam_front) << std::endl;
            os << "up: " << glm::to_string(base_camera.m_cam_up) << std::endl;
            os << "right: " << glm::to_string(base_camera.m_cam_right) << std::endl;
            os << "world_up: " << glm::to_string(base_camera.m_world_up) << std::endl;
            os << "yaw: " << base_camera.m_yaw << std::endl;
            os << "pitch: " << base_camera.m_pitch << std::endl;
            return os;
        }

    protected:

        // Initializes the camera's position, radius, yaw, pitch, front, up and right vectors. This also be used in reset function.
        void init_camera();

        // Update camera orientation using Euler Angles. Update m_cam_front, m_cam_right and m_cam_up Vectors with the updated yaw and pitch values.
        void update_camera_vectors(glm::vec3 front);

    private:
        // camera Attributes
        glm::vec3 m_position{};
        glm::vec3 m_target{};
        glm::vec3 m_cam_front{};
        glm::vec3 m_cam_up{};
        glm::vec3 m_cam_right{};
        glm::vec3 m_world_up{};
        // euler Angles
        float m_yaw{};
        float m_pitch{};
        // camera options
        float m_fov{}; // field of view in degrees
    };
}

#endif // !ORBIT_CAMERA_HPP