#include "plane.hpp"

namespace SignalTracer {
    Plane::Plane(const Plane& other)
        : m_normal{ other.m_normal }
        , m_d{ other.m_d } {}

    Plane::Plane(float normal_x, float normal_y, float normal_z, float d)
        : m_normal{ glm::normalize(glm::vec3{normal_x, normal_y, normal_z}) }
        , m_d{ d } {}

    Plane::Plane(const glm::vec3& normal, float d)
        : m_normal{ glm::normalize(normal) }
        , m_d{ d } {}

    Plane::Plane(const glm::vec3& normal, const glm::vec3& point)
        : m_normal{ glm::normalize(normal) }
        , m_d{ -glm::dot(m_normal, point) } {}

    Plane::Plane(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3)
        : m_normal{ glm::normalize(glm::cross(point2 - point1, point3 - point1)) }
        , m_d{ -glm::dot(m_normal, point1) } {}

    bool Plane::is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const {
        float denom = glm::dot(ray.get_direction(), m_normal);
        // if the ray is parallel to the plane, return false
        if (std::fabs(denom) <= Constant::EPSILON) {
            return false;
        }

        // if the hit point is outside the interval, return false
        float t = (m_d - glm::dot(m_normal, ray.get_origin())) / denom;
        if (!interval.contains(t)) {
            return false;
        }

        record.t = t;
        record.point = ray.point_at(t);
        // record.normal = m_normal;
        record.normal = (glm::dot(ray.get_direction(), m_normal) > 0) ? -m_normal : m_normal;
        return true;
    }

    float Plane::calc_distance(const glm::vec3& point) const {
        return glm::dot(m_normal, point) + m_d;
    }

    float Plane::calc_signed_distance(const glm::vec3& point) const {
        return calc_distance(point);
    }

    float Plane::calc_unsigned_distance(const glm::vec3& point) const {
        return std::fabs(calc_distance(point));
    }

    glm::vec3 Plane::find_closest_point(const glm::vec3& point) const {
        return point - m_normal * calc_signed_distance(point);
    }

    float Plane::dot(const Plane& plane, const glm::vec4& vec4) {
        return glm::dot(plane.m_normal, glm::vec3{ vec4 }) + plane.m_d * vec4.w;
    }

    float Plane::dot(const Plane& plane, const glm::vec3& vec3) {
        return glm::dot(plane.m_normal, vec3) + plane.m_d;
    }

    float Plane::dot_normal(const Plane& plane, const glm::vec3& normal) {
        return glm::dot(plane.m_normal, normal);
    }
}