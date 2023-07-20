#pragma once

#ifndef BVH_MAP_HPP
#define BVH_MAP_HPP

#include "triangle.hpp"
#include "constant.hpp"
#include "hittable_list.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>
#include <vector>

namespace SignalTracer {
    class BVH : public Hittable {
    public:
        BVH() = default;

        BVH(const HittableList& obj_container) : BVH{ obj_container.objects(), 0, obj_container.objects().size(), 0 } {}

        template<typename HittableType>
        BVH(const std::vector<shared_ptr<HittableType>>& src_objects, size_t start, size_t end, size_t level = 0) {
            m_level = level;
            // create a copy of pointers to source objects
            auto objects = src_objects;

            // const int axis = random_int(0, 2);
            const auto axis = level % 3;
            const auto comparator = (axis == 0) ? box_x_compare
                : (axis == 1) ? box_y_compare
                : box_z_compare;

            const auto object_span = end - start;

            if (src_objects.empty() || 0 >= object_span) {
                std::cerr << "No objects in BVH constructor." << std::endl;
                return;
            }
            else if (object_span == 1) {
                m_left = m_right = objects[start];
            }
            else if (object_span == 2) {
                if (comparator(objects[start], objects[start + 1])) {
                    m_left = objects[start];
                    m_right = objects[start + 1];
                }
                else {
                    m_left = objects[start + 1];
                    m_right = objects[start];
                }
            }
            else {
                std::sort(objects.begin() + start, objects.begin() + end, comparator);

                const auto mid = start + object_span / 2;
                m_left = make_shared<BVH>(objects, start, mid, level + 1);
                m_right = make_shared<BVH>(objects, mid, end, level + 1);
            }

            const auto box_left = m_left->bounding_box();
            const auto box_right = m_right->bounding_box();
            m_box = AABB(box_left, box_right);
        }

        // copy constructor
        BVH(const BVH& bvh) : m_left{ bvh.m_left }, m_right{ bvh.m_right }, m_box{ bvh.m_box } {}

        // move constructor
        BVH(BVH&& bvh) noexcept : m_left{ bvh.m_left }, m_right{ bvh.m_right }, m_box{ bvh.m_box } {}

        // assignment operator
        BVH& operator=(const BVH& bvh) {
            m_left = bvh.m_left;
            m_right = bvh.m_right;
            m_box = bvh.m_box;
            return *this;
        }

        // move assignment operator
        BVH& operator=(BVH&& bvh) noexcept {
            m_left = bvh.m_left;
            m_right = bvh.m_right;
            m_box = bvh.m_box;
            return *this;
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
            if (!m_box.is_hit(ray, interval)) {
                return false;
            }

            const auto hit_left = m_left->is_hit(ray, interval, record);
            const auto hit_right = m_right->is_hit(ray, Interval{ interval.min(), hit_left ? record.get_t() : interval.max() }, record);

            return hit_left || hit_right;
        }

        AABB bounding_box() const override { return m_box; }

    private:
        static bool box_compare(const shared_ptr<Hittable> left, const shared_ptr<Hittable> right, int axis_idx) {
            // if (left->bounding_box().empty() || right->bounding_box().empty()) {
            //     throw std::runtime_error("No bounding box in BVH constructor.");
            // }
            return left->bounding_box()[axis_idx].min() < right->bounding_box()[axis_idx].min();
        }

        static bool box_x_compare(const shared_ptr<Hittable> left, const shared_ptr<Hittable> right) {
            return box_compare(left, right, 0);
        }
        static bool box_y_compare(const shared_ptr<Hittable> left, const shared_ptr<Hittable> right) {
            return box_compare(left, right, 1);
        }
        static bool box_z_compare(const shared_ptr<Hittable> left, const shared_ptr<Hittable> right) {
            return box_compare(left, right, 2);
        }


    private:
        shared_ptr<Hittable> m_left{};
        shared_ptr<Hittable> m_right{};
        AABB m_box{};
        size_t m_level{};
    };
}

#endif // !BVH_MAP_HPP