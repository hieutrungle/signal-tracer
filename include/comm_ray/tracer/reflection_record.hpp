#pragma once

#ifndef REFLECTION_RECORD_HPP
#define REFLECTION_RECORD_HPP

#include <glm/gtx/string_cast.hpp>
#include "glm/glm.hpp"
#include <iostream>
#include <memory>

namespace SignalTracer {

    struct ReflectionRecord {
        int reflection_count{};
        std::vector<glm::vec3> ref_points{};
        std::vector<std::shared_ptr<Material>> ref_material_ptrs{};

        // cout overload
        friend std::ostream& operator<<(std::ostream& os, const ReflectionRecord& record) {
            os << "ReflectionRecord: " << std::endl;
            os << "Reflection count: " << record.reflection_count << std::endl;
            os << "Trace path: " << std::endl;
            for (const auto& point : record.ref_points) {
                os << "\t" << glm::to_string(point) << std::endl;
            }
            os << "Trace materials: " << std::endl;
            for (const auto& material_ptr : record.ref_material_ptrs) {
                if (material_ptr == nullptr) {
                    os << "\t" << "nullptr" << std::endl;
                }
                else {
                    os << "\t" << *material_ptr << std::endl;
                }
            }
            return os;
        }
    };
}

#endif // !REFLECTION_RECORD_HPP