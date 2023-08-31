#include "quad.hpp"

namespace SignalTracer {
    Quad::Quad(const glm::vec3& Q, const glm::vec3& u, const glm::vec3& v, std::shared_ptr<Material> mat_ptr)
        : m_q{ Q }, m_u{ u }, m_v{ v }
        , m_normal{ glm::normalize(glm::cross(u, v)) }
        , m_d{ glm::dot(m_normal, Q) }
        , m_w{ glm::cross(u, v) / glm::dot(glm::cross(u, v), glm::cross(u, v)) }
        , m_mat_ptr{ mat_ptr } {
        set_bounding_box();
    }

    void Quad::set_bounding_box() {
        m_box = AABB{ m_q, m_q + m_u + m_v }.pad();
    }

    AABB Quad::bounding_box() const { return m_box; }

    bool Quad::is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const {
        float denom = glm::dot(m_normal, ray.get_direction());

        // No hit if the ray is parallel to the plane.
        if (fabs(denom) < Constant::EPSILON)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        float t = (m_d - glm::dot(m_normal, ray.get_origin())) / denom;
        if (!interval.contains(t))
            return false;

        // Determine the hit point lies within the planar shape using its plane coordinates.
        glm::vec3 intersection_point = ray.point_at(t);

        if (!is_interior(intersection_point)) { return false; }

        // glm::vec3 planar_hitpoint_vec = intersection_point - m_q;
        // auto alpha = glm::dot(m_w, glm::cross(planar_hitpoint_vec, m_v));
        // auto beta = glm::dot(m_w, glm::cross(m_u, planar_hitpoint_vec));

        // if (!is_interior(alpha, beta, record))
        //     return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        record.t = t;
        record.point = intersection_point;
        record.mat_ptr = m_mat_ptr;
        record.normal = (glm::dot(ray.get_direction(), m_normal) > 0) ? -m_normal : m_normal;

        return true;
    }

    bool Quad::is_interior(const glm::vec3& point) const {
        glm::vec3 planar_hitpoint_vec = point - m_q;
        auto alpha = glm::dot(m_w, glm::cross(planar_hitpoint_vec, m_v));
        auto beta = glm::dot(m_w, glm::cross(m_u, planar_hitpoint_vec));

        if (alpha < 0 || alpha > 1 || beta < 0 || beta > 1)
            return false;
        return true;
    }
}