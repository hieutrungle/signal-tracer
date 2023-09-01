/*
Written by Hieu Le

Part of this is taken from the following source:
https://graphics.stanford.edu/~mdfisher/Code/Engine/Plane.cpp.html
Written by Matthew Fisher

A standard 3D plane (space plane.)  Essentially just the surface defined by a*x + b*y + c*z + d = 0
See Plane.h for a description of these functions.
*/

#include "constant.hpp"
#include "hittable.hpp"
#include "glm/glm.hpp"
#include <math.h>

namespace SignalTracer {
    class Plane : public Hittable {
    public:
        Plane() = default;
        Plane(const Plane& P);
        Plane(float a, float b, float c, float d);
        Plane(const glm::vec3& normal, float d);
        Plane(const glm::vec3& point, const glm::vec3& normal);
        Plane(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3);

        virtual glm::vec3 get_normal() const { return m_normal; }
        virtual float get_d() const { return m_d; }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;
        static bool is_hit_pplane(const Plane& plane1, const Plane& plane2, IntersectRecord& record);
        AABB bounding_box() const override { return AABB{}; };
        std::ostream& print(std::ostream& out) const override;

        float calc_distance(const glm::vec3& point) const;
        float calc_signed_distance(const glm::vec3& point) const;
        float calc_unsigned_distance(const glm::vec3& point) const;

        glm::vec3 find_closest_point(const glm::vec3& point) const;

        //dot product of a plane and a 4D vector
        static float dot(const Plane& plane, const glm::vec4& vec4);
        //dot product of a plane and a 3D coordinate
        static float dot(const Plane& plane, const glm::vec3& vec3);
        //dot product of a plane and a 3D normal
        static float dot_normal(const Plane& plane, const glm::vec3& normal);

    private:
        glm::vec3 m_normal{};
        float m_d{ 0.0f };
    };
}