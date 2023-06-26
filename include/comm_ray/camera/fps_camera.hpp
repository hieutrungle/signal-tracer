#pragma once 
#ifndef FPS_CAMERA_HPP
#define FPS_CAMERA_HPP

#include "base_camera.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


class FPSCamera : public BaseCamera {
public:
    // constructor with vectors
    FPSCamera() = default;

    // FPSCamera(
    //     glm::vec3 position = CameraDefaultParam::position,
    //     glm::vec3 target = CameraDefaultParam::target,
    //     glm::vec3 world_up = CameraDefaultParam::world_up,
    //     float speed = CameraDefaultParam::speed,
    //     float mouse_sensitivity = CameraDefaultParam::mouse_sensitivity,
    //     float zoom = CameraDefaultParam::zoom
    // )
    //     : BaseCamera(position, target, world_up, speed, mouse_sensitivity, zoom) {
    //     update_camera_vectors();
    // }
};

#endif // !FPS_CAMERA_HPP