#pragma once

#ifndef _COMM_RAY_TRACER_TRACER_INTERFACE_HPP_
#define _COMM_RAY_TRACER_TRACER_INTERFACE_HPP_

#include "path_record.hpp"
#include "glm/glm.hpp"
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
        virtual void trace_rays(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) = 0;

        virtual std::ostream& print(std::ostream& out) const {
            out << "Signal Tracing: \n ";
            return out;
        }
        friend std::ostream& operator<<(std::ostream& out, const TracerInterface& signal_tracer) {
            return signal_tracer.print(out);
        }

    };
}

#endif //! _COMM_RAY_TRACER_TRACER_INTERFACE_HPP_