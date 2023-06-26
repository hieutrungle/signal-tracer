#pragma once

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "ray.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>

namespace signal_tracer {
    class IntersectRecord;

    class Material {
    public:
        Material() = default;
        virtual ~Material() = default;

        virtual bool scatter(const Ray& UTILS_UNUSED_PARAM(ray_in), const IntersectRecord& UTILS_UNUSED_PARAM(record), glm::vec3& UTILS_UNUSED_PARAM(attenuation), Ray& UTILS_UNUSED_PARAM(scattered)) const {
            return false;
        }
    };
}

#endif // !MATERIAL_HPP