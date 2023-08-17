#pragma once

#ifndef _COMM_RAY_TRACER_BASE_TRACER_HPP_
#define _COMM_RAY_TRACER_BASE_TRACER_HPP_

#include "glm/glm.hpp"
#include "bvh_map.hpp"
#include "tracer_interface.hpp"

namespace SignalTracer {

    class BaseTracer : public TracerInterface {
    public:
        BaseTracer() = default;

        BaseTracer(const std::vector<Model>& models, int max_reflection = 2);

        BaseTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2);

        virtual ~BaseTracer() = default;

        //copy constructor
        BaseTracer(const BaseTracer& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_bvhs{ other.m_bvhs }
            , m_tlas{ other.m_tlas } {}

        //copy assignment
        BaseTracer& operator=(const BaseTracer& other) {
            m_max_reflection = other.m_max_reflection;
            m_bvhs = other.m_bvhs;
            m_tlas = other.m_tlas;
            return *this;
        }

        // move constructor
        BaseTracer(BaseTracer&& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_bvhs{ other.m_bvhs }
            , m_tlas{ other.m_tlas } {}

        // move assignment
        BaseTracer& operator=(BaseTracer&& other) noexcept {
            m_max_reflection = other.m_max_reflection;
            m_bvhs = other.m_bvhs;
            m_tlas = other.m_tlas;
            return *this;
        }

    protected:
        int m_max_reflection{ 2 };
        std::vector<BVHInstance> m_bvhs{};
        TLAS m_tlas{};
    };

}

#endif //! _COMM_RAY_TRACER_BASE_TRACER_HPP_