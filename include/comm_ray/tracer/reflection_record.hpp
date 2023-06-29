#pragma once

#ifndef REFLECTION_RECORD_HPP
#define REFLECTION_RECORD_HPP

#include <glm/gtx/string_cast.hpp>
#include "glm/glm.hpp"
#include <iostream>

namespace signal_tracer {

    struct ReflectionRecord {
        int reflection_count{};
        std::vector<glm::vec3> ref_points{};

        // cout overload
        friend std::ostream& operator<<(std::ostream& os, const ReflectionRecord& record) {
            os << "ReflectionRecord: " << std::endl;
            os << "Reflection count: " << record.reflection_count << std::endl;
            os << "Trace path: " << std::endl;
            for (const auto& point : record.ref_points) {
                os << "\t" << glm::to_string(point) << std::endl;
            }
            return os;
        }
    };
}

#endif // !REFLECTION_RECORD_HPP