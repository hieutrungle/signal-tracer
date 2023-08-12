#pragma once

#ifndef INTERSECT_RECORD_HPP
#define INTERSECT_RECORD_HPP

#include "ray.hpp"
#include "constant.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <memory>

namespace SignalTracer {
    class Material;
    class Triangle;

    /*
        ----------------------------------------
        IntersectRecord
        A container to store the information of the intersection
        ----------------------------------------
    */
    struct IntersectRecord {
        glm::vec3 point{ Constant::INF_NEG };
        glm::vec3 normal{};
        std::shared_ptr<Material> mat_ptr{ nullptr };
        std::shared_ptr<Triangle> tri_ptr{ nullptr };
        float t{ Constant::INF_POS };
        bool front_face{};

        friend std::ostream& operator<<(std::ostream& os, const IntersectRecord& record) {
            os << "IntersectRecord: " << glm::to_string(record.point) << std::endl;
            return os;
        }
    };
}

#endif // !INTERSECT_RECORD_HPP