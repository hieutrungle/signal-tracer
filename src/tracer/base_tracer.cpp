#include "base_tracer.hpp"

namespace SignalTracer {
    BaseTracer::BaseTracer(const std::vector<Model>& models, int max_reflection)
        : m_max_reflection{ max_reflection } {
        m_bvhs.reserve(models.size() * 16);
        for (std::size_t i = 0; i < models.size(); ++i) {
            std::shared_ptr<BVHAccel> bvh_ptr{ std::make_shared<BVHAccel>(models[i]) };

            // Create intances
            for (int j = 0; j < 1; ++j) {
                glm::mat4 trans{ glm::mat4(1.0f) };
                BVHInstance bvh_inst{ bvh_ptr, trans };
                m_bvhs.emplace_back(bvh_inst);
            }
        }
        m_tlas = TLAS{ m_bvhs, static_cast<uint>(m_bvhs.size()) };
        m_tlas.build();
    }

    BaseTracer::BaseTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection)
        : m_max_reflection{ max_reflection } {
        m_bvhs.reserve(models.size() * 16);
        for (std::size_t i = 0; i < models.size(); ++i) {
            std::shared_ptr<BVHAccel> bvh_ptr{ std::make_shared<BVHAccel>(models[i].get()) };

            // Create intances, increase j to create more instances with transformation trans
            for (int j = 0; j < 1; ++j) {
                glm::mat4 trans{ glm::mat4(1.0f) };
                BVHInstance bvh_inst{ bvh_ptr, trans };
                m_bvhs.emplace_back(bvh_inst);
            }
        }
        m_tlas = TLAS{ m_bvhs, static_cast<uint>(m_bvhs.size()) };
        m_tlas.build();
    }
}