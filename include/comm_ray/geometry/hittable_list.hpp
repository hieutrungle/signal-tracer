#pragma once
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "interval.hpp"
#include "hittable.hpp"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

namespace SignalTracer {
    class HittableList : public Hittable {
    public:
        HittableList() = default;

        template<typename HittableType>
        HittableList(const std::vector<shared_ptr<HittableType>> objects) {
            for (const auto& object : objects) {
                add(object);
            }
        };

        HittableList(const shared_ptr<Hittable> object) { add(object); };

        void clear() { m_objects.clear(); };
        void add(shared_ptr<Hittable> object) {
            m_objects.push_back(object);
            m_box = AABB{ m_box, object->bounding_box() };
        };

        bool is_hit(const Ray& r, const Interval& interval, IntersectRecord& rec) const override {
            IntersectRecord temp_rec{};
            bool is_intersect{ false };
            auto closest_so_far{ interval.max() };

            for (const auto& object : m_objects) {
                Interval interval_tmp(interval.min(), closest_so_far);
                if (object->is_hit(r, interval_tmp, temp_rec)) {
                    is_intersect = true;
                    closest_so_far = temp_rec.get_t();
                    rec = temp_rec;
                }
            }

            return is_intersect;
        };

        const std::vector<shared_ptr<Hittable>>& objects() const {
            return m_objects;
        };

        AABB bounding_box() const override {
            if (m_objects.empty()) {
                return AABB{};
            }
            return m_box;
        };

    private:
        std::vector<shared_ptr<Hittable>> m_objects{};
        AABB m_box{};
    };
}

#endif // !HITTABLE_LIST_H