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


        /// @brief Initialize TLAS and BLAS (BVH) struture
        /// @param models Imported ASSIMP models that contains meshes
        BaseTracer(const std::vector<Model>& models);

        /// @brief Initialize TLAS and BLAS (BVH) struture
        /// @param models Imported ASSIMP models that contains meshes
        BaseTracer(const std::vector<std::reference_wrapper<Model>>& models);

        virtual ~BaseTracer() = default;

        //copy constructor
        BaseTracer(const BaseTracer& other)
            : m_bvhs{ other.m_bvhs }
            , m_tlas{ other.m_tlas } {}

        //copy assignment
        BaseTracer& operator=(const BaseTracer& other) {
            m_bvhs = other.m_bvhs;
            m_tlas = other.m_tlas;
            return *this;
        }

        // move constructor
        BaseTracer(BaseTracer&& other)
            : m_bvhs{ other.m_bvhs }
            , m_tlas{ other.m_tlas } {}

        // move assignment
        BaseTracer& operator=(BaseTracer&& other) noexcept {
            m_bvhs = other.m_bvhs;
            m_tlas = other.m_tlas;
            return *this;
        }

    protected:
        std::vector<BVHInstance> m_bvhs{};
        TLAS m_tlas{};
    };

}

#endif //! _COMM_RAY_TRACER_BASE_TRACER_HPP_