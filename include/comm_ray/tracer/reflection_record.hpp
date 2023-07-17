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
        float signal_loss{};
        float signal_strength{};
        float signal_delay{};
        float transmitting_distance{};
        float signal_phase{};

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
                    os << "\t" << *material_ptr;
                }
            }
            os << "Signal loss: " << "\t" << record.signal_loss << " dB" << std::endl;
            os << "Signal strength: " << "\t" << record.signal_strength << " dBm" << std::endl;
            os << "Transmitting distance: " << "\t" << record.transmitting_distance << " m" << std::endl;
            os << "Signal delay: " << "\t" << record.signal_delay << " s" << std::endl;
            os << "Signal phase: " << "\t" << record.signal_phase << std::endl;
            return os;
        }
    };
}

#endif // !REFLECTION_RECORD_HPP