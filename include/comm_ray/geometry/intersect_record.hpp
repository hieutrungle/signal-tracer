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
        glm::vec3 point{ Constant::INF_NEG };           // 12 bytes
        glm::vec3 normal{};                             // 12 bytes
        std::shared_ptr<Triangle> tri_ptr{ nullptr };   // 16 bytes
        float t{ Constant::INF_POS };                   // 4 bytes
        float reserved{ 0.0f };                         // 4 bytes
        friend std::ostream& operator<<(std::ostream& os, const IntersectRecord& record) {
            os << "IntersectRecord: " << glm::to_string(record.point) << std::endl;
            return os;
        }
    };
}

#endif // !INTERSECT_RECORD_HPP