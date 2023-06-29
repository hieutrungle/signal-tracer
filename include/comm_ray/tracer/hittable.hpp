#pragma once
#ifndef HITTABLE_H
#define HITTABLE_H

#include "intersect_record.hpp"
#include "interval.hpp"
#include "aabb.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>

namespace signal_tracer {
    class Hittable {
    public:
        virtual bool intersect(const Ray& ray, const Interval& interval, IntersectRecord& record) const = 0;
        virtual AABB bounding_box() const = 0;

        virtual std::ostream& print(std::ostream& out) const {
            return out;
        }
        friend std::ostream& operator<<(std::ostream& out, const Hittable& hittable) {
            return hittable.print(out);
        }
    };
}


#endif // !HITTABLE_H