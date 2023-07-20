#pragma once

#ifndef _COMM_RAY_TRACER_TRACER_INTERFACE_HPP_
#define _COMM_RAY_TRACER_TRACER_INTERFACE_HPP_

#include "reflection_record.hpp"
#include "model.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include "reflection_record.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace SignalTracer {
    class TracerInterface {
    public:
        TracerInterface() = default;
        virtual ~TracerInterface() = default;
        TracerInterface(const TracerInterface&) = delete;
        TracerInterface(TracerInterface&&) = delete;
        TracerInterface& operator=(const TracerInterface&) = delete;
        TracerInterface& operator=(TracerInterface&&) = delete;

        virtual void reset() = 0;
        virtual void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) = 0;

        virtual std::ostream& print(std::ostream& out) const {
            out << "Signal Tracing: \n ";
            return out;
        }
        friend std::ostream& operator<<(std::ostream& out, const TracerInterface& signal_tracer) {
            return signal_tracer.print(out);
        }

    protected:
        virtual std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<Model>& models) = 0;
        virtual std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<std::reference_wrapper<Model>>& models) = 0;
    };
}

#endif //! _COMM_RAY_TRACER_TRACER_INTERFACE_HPP_