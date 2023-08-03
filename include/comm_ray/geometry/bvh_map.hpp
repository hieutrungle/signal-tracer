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

#define BVH3

namespace SignalTracer {



#ifdef BVH1
    class BVHAccel : public Hittable {
    public:
        BVHAccel() = default;

        BVHAccel(const HittableList& obj_container) : BVHAccel{ obj_container.objects(), 0, obj_container.objects().size(), 0 } {}

        BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end, size_t level = 0) {
            partition_node(src_objects, start, end, level);
        }

        // copy constructor
        BVHAccel(const BVHAccel& bvh) : m_left{ bvh.m_left }, m_right{ bvh.m_right }, m_box{ bvh.m_box } {}

        // move constructor
        BVHAccel(BVHAccel&& bvh) noexcept : m_left{ bvh.m_left }, m_right{ bvh.m_right }, m_box{ bvh.m_box } {}

        // assignment operator
        BVHAccel& operator=(const BVHAccel& bvh) {
            m_left = bvh.m_left;
            m_right = bvh.m_right;
            m_box = bvh.m_box;
            return *this;
        }

        // move assignment operator
        BVHAccel& operator=(BVHAccel&& bvh) noexcept {
            m_left = bvh.m_left;
            m_right = bvh.m_right;
            m_box = bvh.m_box;
            return *this;
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
            Interval tmp_interval{ interval };
            if (!m_box.is_hit(ray, tmp_interval)) {
                return false;
            }

            const auto hit_left = m_left->is_hit(ray, interval, record);
            const auto hit_right = m_right->is_hit(ray, Interval{ interval.min(), hit_left ? record.get_t() : interval.max() }, record);

            return hit_left || hit_right;
        }

        AABB bounding_box() const override { return m_box; }

    private:
        // void partition_node(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end, size_t level = 0, const std::size_t& num_buckets = 32) {
        //     m_level = level;
        //     // create a copy of pointers to source objects
        //     std::vector<shared_ptr<Hittable>> objects{ std::vector<shared_ptr<Hittable>>(&src_objects[start],&src_objects[end]) };

        //     glm::vec3 centroid_min{ Constant::INFINITY_POS };
        //     glm::vec3 centroid_max{ Constant::INFINITY_NEG };
        //     for (const auto& object : objects) {
        //         const auto box = object->bounding_box();
        //         centroid_min = glm::min(centroid_min, box.get_centroid());
        //         centroid_max = glm::max(centroid_max, box.get_centroid());
        //     }
        //     std::cout << "Level: " << level << "; centroid_min: " << glm::to_string(centroid_min) << "; centroid_max: " << glm::to_string(centroid_max) << std::endl;

        //     float best_sah{ Constant::INFINITY_POS };
        //     AABB best_left_bbox{}, best_right_bbox{};
        //     std::size_t best_left_count{};
        //     int best_axis{};
        //     int best_pplane{};
        //     float best_centroid_max{}, best_centroid_min{};

        //     for (int axis = 0; axis < 3; ++axis) {

        //         // Skip if the centroids are the same
        //         if (centroid_max[axis] - centroid_min[axis] <= Constant::EPSILON) {
        //             continue;
        //         }

        //         std::vector<AABB> boxes(num_buckets, AABB{});
        //         std::vector<std::size_t> primitive_counts(num_buckets, 0);

        //         partition_buckets(objects, centroid_min, centroid_max, axis, boxes, primitive_counts);

        //         for (std::size_t plane_idx = 1; plane_idx < num_buckets; ++plane_idx) {
        //             AABB left_box{};
        //             AABB right_box{};
        //             std::size_t left_count{ 0 };
        //             std::size_t right_count{ 0 };

        //             for (std::size_t i = 0; i < plane_idx; ++i) {
        //                 left_box.expand(boxes[i]);
        //                 left_count += primitive_counts[i];
        //             }

        //             for (std::size_t i = plane_idx; i < num_buckets; ++i) {
        //                 right_box.expand(boxes[i]);
        //                 right_count += primitive_counts[i];
        //             }

        //             if (left_count == 0 || right_count == 0) {
        //                 continue;
        //             }

        //             // We remove some constant from the SAH equation.
        //             // Those are:
        //             // 1. The cost of traversing the BVH tree
        //             // 2. The cost of intersecting a primitive
        //             // 3. The cost of intersecting a BVH node
        //             // 4. The surface area of the bounding box of the current BVH node
        //             const auto sah = left_count * left_box.calc_surface_area() + right_count * right_box.calc_surface_area();

        //             if (sah < best_sah) {

        //                 best_sah = sah;
        //                 best_left_bbox = left_box;
        //                 best_right_bbox = right_box;
        //                 best_left_count = left_count;
        //                 best_axis = axis;
        //                 best_pplane = plane_idx;
        //                 best_centroid_max = centroid_max[axis];
        //                 best_centroid_min = centroid_min[axis];
        //                 std::cout << "Level: " << level << ". SAH: " << sah << "; axis: " << axis << "; plane_idx: " << plane_idx << "; left_count: " << left_count << "; right_count: " << right_count << "; left_box: " << left_box << "; right_box: " << right_box << std::endl;
        //             }
        //         }
        //     }

        //     const auto object_span = objects.size();
        //     std::size_t mid{};

        //     if (best_sah == Constant::INFINITY_POS) {
        //         // All centroids are at the same position
        //         std::cout << "Level: " << level << ". All centroids are at the same position." << std::endl;
        //         mid = object_span / 2;
        //     }
        //     else {
        //         // lambda function
        //         auto get_left_primitives = [&best_axis, &best_pplane, &best_centroid_min, &best_centroid_max, this](const shared_ptr<Hittable>& object) {
        //             AABB box = object->bounding_box();
        //             glm::vec3 centroid = box.get_centroid();
        //             int bucket_idx = calc_bucket_idx(best_centroid_min, best_centroid_max, centroid[best_axis], best_pplane);
        //             return bucket_idx < best_pplane;
        //             };
        //         std::partition(objects.begin(), objects.end(), get_left_primitives);
        //         mid = best_left_count;
        //     }

        //     if (objects.empty() || object_span == 0) {
        //         std::cerr << "No objects in BVH constructor." << std::endl;
        //         return;
        //     }
        //     else if (object_span == 1) {
        //         m_left = m_right = objects[0];
        //     }
        //     else if (object_span == 2) {
        //         m_left = objects[0];
        //         m_right = objects[1];
        //     }
        //     else {
        //         m_left = make_shared<BVHAccel>(objects, 0, mid, level + 1);
        //         m_right = make_shared<BVHAccel>(objects, mid, objects.size(), level + 1);
        //     }

        //     const auto box_left = m_left->bounding_box();
        //     const auto box_right = m_right->bounding_box();
        //     m_box = AABB(box_left, box_right);
        //     std::cout << "Level: " << level << ". Box: " << m_box << std::endl;
        // }

        void partition_buckets(
            const std::vector<std::shared_ptr<Hittable>>& objects,
            const glm::vec3& centroid_min,
            const glm::vec3& centroid_max,
            const int& axis,
            std::vector<AABB>& boxes,
            std::vector<std::size_t>& primitive_counts
        ) {
            const std::size_t num_buckets = boxes.size();

            for (const auto& object : objects) {
                const AABB box = object->bounding_box();
                const glm::vec3 centroid = box.get_centroid();

                const auto bucket_idx = calc_bucket_idx(centroid_min[axis], centroid_max[axis], centroid[axis], num_buckets);

                boxes[bucket_idx].expand(box);
                primitive_counts[bucket_idx]++;
            }
        }

        inline int calc_bucket_idx(const float centroid_min, const float centroid_max, const float centroid, const int num_buckets) {
            const float numerator = centroid - centroid_min;
            const float denominator = centroid_max - centroid_min + Constant::EPSILON;
            return static_cast<std::size_t>(num_buckets * (numerator / denominator));
        }


        void partition_node(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end, size_t level = 0) {
            m_level = level;
            // create a copy of pointers to source objects
            std::vector<shared_ptr<Hittable>> objects{ std::vector<shared_ptr<Hittable>>(&src_objects[start],&src_objects[end]) };

            // const int axis = random_int(0, 2);
            const auto axis = level % 3;
            const auto comparator = (axis == 0) ? box_x_compare
                : (axis == 1) ? box_y_compare
                : box_z_compare;

            const auto object_span = objects.size();

            std::sort(objects.begin(), objects.end(), comparator);

            if (src_objects.empty() || 0 >= object_span) {
                std::cerr << "No objects in BVH constructor." << std::endl;
                return;
            }
            else if (object_span == 1) {
                m_left = m_right = objects[0];
            }
            else if (object_span == 2) {
                m_left = objects[0];
                m_right = objects[1];
            }
            else {
                const std::size_t mid = object_span / 2;
                m_left = make_shared<BVHAccel>(objects, 0, mid, level + 1);
                m_right = make_shared<BVHAccel>(objects, mid, objects.size(), level + 1);
            }

            const auto box_left = m_left->bounding_box();
            const auto box_right = m_right->bounding_box();
            m_box = AABB(box_left, box_right);
        }

        static bool box_compare(const shared_ptr<Hittable> left, const shared_ptr<Hittable> right, int axis_idx) {
            // if (left->bounding_box().empty() || right->bounding_box().empty()) {
            //     throw std::runtime_error("No bounding box in BVH constructor.");
            // }
            return left->bounding_box().get_min()[axis_idx] < right->bounding_box().get_min()[axis_idx];
            // return left->bounding_box()[axis_idx].min() < right->bounding_box()[axis_idx].min();
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

#elif defined(BVH2)

    class BVHNode {
    public:

        BVHNode(const AABB& box, std::size_t start, std::size_t range)
            : m_box{ box }, m_start{ start }, m_range{ range }, m_left{ nullptr }, m_right{ nullptr } {}

        inline bool is_leaf() const { return m_left == nullptr && m_right == nullptr; }

        AABB m_box{}; ///< bounding box ofthe node
        std::size_t m_start{}; ///< start index into the primitive list
        std::size_t m_range{}; ///< range of index into the primitive list
        std::shared_ptr<BVHNode> m_left{}; ///< left child node
        std::shared_ptr<BVHNode> m_right{}; ///< right child node
    };

    class BVHAccel : public Hittable {
    public:
        BVHAccel() = default;

        BVHAccel(const HittableList& obj_container) : BVHAccel{ obj_container.objects(), 0, obj_container.objects().size(), 0 } {}

        BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range, const std::size_t& max_leaf_size = 2, std::size_t level = 0)
            : m_primitives{ std::vector<shared_ptr<Hittable>>(&src_objects[start],&src_objects[range]) }
            , m_root{ std::make_shared<BVHNode>(AABB{}, start, range) } {
            if (m_primitives.empty() || m_primitives.size() == 0) {
                std::cerr << "No objects in BVH constructor." << std::endl;
                return;
            }

            // std::cout << "Constructing BVH..." << std::endl;

            partition(m_root, m_primitives, start, range, max_leaf_size, level);
            // std::cout << "BVH constructed." << std::endl;
        }

        void partition(const shared_ptr<BVHNode>& bvh_node, std::vector<shared_ptr<Hittable>>& src_objects, std::size_t start, std::size_t range, std::size_t max_leaf_size = 2, std::size_t level = 0) {

            auto start_iter = src_objects.begin() + start;
            auto end_iter = src_objects.begin() + start + range;

            // const int axis = random_int(0, 2);
            const auto axis = level % 3;
            const auto comparator = (axis == 0) ? box_x_compare
                : (axis == 1) ? box_y_compare
                : box_z_compare;

            std::sort(start_iter, end_iter, comparator);

            if (range <= max_leaf_size) {

                // iter loop
                for (auto iter = start_iter; iter != end_iter; ++iter) {
                    bvh_node->m_box = AABB(bvh_node->m_box, (*iter)->bounding_box());
                }
                return;
            }

            const std::size_t mid = start + (range / 2);

            bvh_node->m_left = std::make_shared<BVHNode>(AABB{}, start, mid - start);
            bvh_node->m_right = std::make_shared<BVHNode>(AABB{}, mid, start + range - mid);

            partition(bvh_node->m_left, src_objects, start, mid - start, max_leaf_size, level + 1);
            partition(bvh_node->m_right, src_objects, mid, start + range - mid, max_leaf_size, level + 1);

            const auto box_left = bvh_node->m_left->m_box;
            const auto box_right = bvh_node->m_right->m_box;
            bvh_node->m_box = AABB(box_left, box_right);
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
            Interval tmp_interval{ interval };
            if (!m_root->m_box.is_hit(ray, tmp_interval)) {
                return false;
            }

            find_closest_hit(ray, interval, record, m_root);
            if (record.is_empty()) {
                return false;
            }
            return true;
        }

        AABB bounding_box() const override { return m_root->m_box; }

    private:
        void find_closest_hit(const Ray& ray, const Interval& interval, IntersectRecord& record, const shared_ptr<BVHNode>& node) const {

            Interval tmp_interval{ interval };
            if (!node->m_box.is_hit(ray, tmp_interval)) {
                return;
            }

            if (node->is_leaf()) {
                for (std::size_t i = node->m_start; i < node->m_start + node->m_range; ++i) {
                    IntersectRecord tmp_record{};
                    if (m_primitives[i]->is_hit(ray, interval, tmp_record)) {
                        if (tmp_record.get_t() < record.get_t()) {
                            record = tmp_record;
                        }
                    }
                }
            }
            else {
                Interval left_interval{ interval };
                Interval right_interval{ interval };

                const bool hit_left = node->m_left->m_box.is_hit(ray, left_interval);
                const bool hit_right = node->m_right->m_box.is_hit(ray, right_interval);
                if (hit_left) {}
                if (hit_right) {}

                std::shared_ptr<BVHNode> first_node_ptr{};
                std::shared_ptr<BVHNode> second_node_ptr{};
                double second_t_min{};
                if (left_interval.min() <= right_interval.min()) {
                    first_node_ptr = node->m_left;
                    second_node_ptr = node->m_right;
                    second_t_min = right_interval.min();
                }
                else {
                    first_node_ptr = node->m_right;
                    second_node_ptr = node->m_left;
                    second_t_min = left_interval.min();
                }

                //! Slow
                find_closest_hit(ray, interval, record, first_node_ptr);
                if (second_t_min < record.get_t() || record.is_empty()) {
                    find_closest_hit(ray, interval, record, second_node_ptr);
                }

            }
        }

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
        std::vector<shared_ptr<Hittable>> m_primitives{};
        shared_ptr<BVHNode> m_root{};
    };

#elif defined(BVH3)

    // struct BVHNode {
    //     glm::vec3 aabb_min{}, aabb_max{};
    //     uint32_t left_node{};
    //     uint32_t first_tri_idx{}, tri_count{};

    //     // cout
    //     std::ostream& print(std::ostream& out) const {
    //         out << "BVHNode: " << std::endl;
    //         out << "aabb_min: " << glm::to_string(aabb_min) << std::endl;
    //         out << "aabb_max: " << glm::to_string(aabb_max) << std::endl;
    //         out << "left_node: " << left_node << std::endl;
    //         out << "first_tri_idx: " << first_tri_idx << std::endl;
    //         out << "tri_count: " << tri_count << std::endl;
    //         return out;
    //     }

    //     friend std::ostream& operator<<(std::ostream& out, const BVHNode& node) {
    //         return node.print(out);
    //     }
    // };

    // class BVHAccel : public Hittable {
    // public:
    //     BVHAccel(const HittableList& obj_container) : BVHAccel{ obj_container.objects(), 0, obj_container.objects().size() } {}

    //     // BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range, const std::size_t& max_leaf_size = 2, std::size_t level = 0)
    //     BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range)
    //         : m_primitives{ std::vector<shared_ptr<Hittable>>(&src_objects[start],&src_objects[range]) }
    //         , m_prim_indices{ std::vector<uint>(m_primitives.size()) }
    //         , m_nodes{ std::vector<BVHNode>(2 * m_primitives.size() - 1) } {

    //         for (std::size_t i = 0; i < m_primitives.size(); ++i) {
    //             m_prim_indices[i] = i;
    //         }

    //         if (m_primitives.empty() || m_primitives.size() == 0) {
    //             std::cerr << "No objects in BVH constructor." << std::endl;
    //             return;
    //         }

    //         auto& root = m_nodes[0];
    //         root.left_node = 0;
    //         root.first_tri_idx = 0;
    //         root.tri_count = m_primitives.size();

    //         update_node_bounds(m_root_idx);

    //         // std::cout << root << std::endl;

    //         subdivide(m_root_idx);
    //     }

    //     void update_node_bounds(const uint node_idx) {
    //         BVHNode& node = m_nodes[node_idx];
    //         node.aabb_min = glm::vec3{ Constant::INFINITY_POS };
    //         node.aabb_max = glm::vec3{ Constant::INFINITY_NEG };
    //         for (uint i = node.first_tri_idx; i < node.first_tri_idx + node.tri_count; ++i) {
    //             uint prim_idx = m_prim_indices[i];
    //             const auto& prim = m_primitives[prim_idx];

    //             node.aabb_min = glm::min(node.aabb_min, prim->get_min());
    //             node.aabb_max = glm::max(node.aabb_max, prim->get_max());
    //         }
    //     }

    //     void subdivide(uint node_idx, uint depth = 0) {

    //         // terminate if reaches leaf node
    //         BVHNode& node = m_nodes[node_idx];
    //         if (node.tri_count <= 2) {
    //             // std::cout << "Depth: " << depth << "; Node idx: " << node_idx << "; Tri count: " << node.tri_count << "; at leaf node, going return //" << std::endl;
    //             return;
    //         }

    //         // 1. Split plane
    //         glm::vec3 extent = node.aabb_max - node.aabb_min;
    //         int axis = 0;
    //         if (extent.y > extent.x) { axis = 1; }
    //         if (extent.z > extent[axis]) { axis = 2; }

    //         // Find split position
    //         float centroid_min = Constant::INFINITY_POS;
    //         float centroid_max = Constant::INFINITY_NEG;
    //         for (uint i = node.first_tri_idx; i < node.first_tri_idx + node.tri_count; ++i) {
    //             uint prim_idx = m_prim_indices[i];
    //             const auto& prim = m_primitives[prim_idx];
    //             glm::vec3 centroid = prim->get_centroid();
    //             centroid_min = glm::min(centroid_min, centroid[axis]);
    //             centroid_max = glm::max(centroid_max, centroid[axis]);
    //         }
    //         float split_pos = (centroid_min + centroid_max) * 0.5f;

    //         // 2. Split into 2 halves
    //         int i = node.first_tri_idx;
    //         int j = node.first_tri_idx + node.tri_count - 1;
    //         while (i <= j) {
    //             const auto& prim = m_primitives[m_prim_indices[i]];
    //             glm::vec3 centroid = prim->get_centroid();
    //             if (centroid[axis] < split_pos) {
    //                 ++i;
    //             }
    //             else {
    //                 std::swap(m_prim_indices[i], m_prim_indices[j]);
    //                 --j;
    //             }
    //         }

    //         // 3. Create child ndoes for each half
    //         uint left_count = i - node.first_tri_idx;
    //         if (left_count == 0 || left_count == node.tri_count) {
    //             // No split occured
    //             // std::cout << "Depth: " << depth << "; no split occured" << "; Node idx: " << node_idx << "; Tri count: " << node.tri_count << "; going return //" << std::endl;
    //             return;
    //         }

    //         // Create child nodes
    //         uint left_child_idx = m_node_used++;
    //         uint right_child_idx = m_node_used++;
    //         m_nodes[left_child_idx].first_tri_idx = node.first_tri_idx;
    //         m_nodes[left_child_idx].tri_count = left_count;
    //         m_nodes[right_child_idx].first_tri_idx = i;
    //         m_nodes[right_child_idx].tri_count = node.tri_count - left_count;

    //         // Update bounds

    //         update_node_bounds(left_child_idx);
    //         update_node_bounds(right_child_idx);

    //         // std::cout << "Depth: " << depth << "; Bounds: " << glm::to_string(node.aabb_min) << " - " << glm::to_string(node.aabb_max) << "; count: " << node.tri_count << "; Node idx: " << node_idx;

    //         node.left_node = left_child_idx;
    //         node.tri_count = 0;

    //         // std::cout << "; Tri count: " << node.tri_count << "\n";

    //         // 4. Recursively subdivide
    //         // std::cout << "Depth: " << depth << " going left" << ", splitting axis: " << axis << std::endl;
    //         subdivide(left_child_idx, depth + 1);
    //         // std::cout << "Depth: " << depth << " going right" << ", splitting axis: " << axis << std::endl;
    //         subdivide(right_child_idx, depth + 1);

    //         // std::cout << "Depth: " << depth << " going up |" << std::endl;

    //     }

    //     AABB bounding_box() const override {
    //         return AABB{ m_nodes[m_root_idx].aabb_min, m_nodes[m_root_idx].aabb_max };
    //     }

    //     bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
    //         return is_hit_(ray, interval, record, m_root_idx);
    //     }

    //     bool is_hit_(const Ray& ray, const Interval& interval, IntersectRecord& record, const uint node_idx) const {
    //         const BVHNode& node = m_nodes[node_idx];
    //         AABB box{ node.aabb_min, node.aabb_max };
    //         Interval tmp_interval{ interval };
    //         if (!box.is_hit(ray, tmp_interval)) {
    //             return false;
    //         }

    //         bool hit_flag = false;
    //         if (node.tri_count > 0) {
    //             for (uint i = 0; i < node.tri_count; ++i) {
    //                 const uint prim_idx = m_prim_indices[node.first_tri_idx + i];
    //                 const auto& prim = m_primitives[prim_idx];

    //                 IntersectRecord tmp_record{};
    //                 if (prim->is_hit(ray, interval, tmp_record)) {
    //                     if (tmp_record.get_t() < record.get_t()) {
    //                         record = tmp_record;
    //                         hit_flag = true;
    //                     }
    //                 }

    //                 // if (prim->is_hit(ray, tmp_interval, record)) {
    //                 //     tmp_interval = Interval{ tmp_interval.min(), tmp_interval.max() < record.get_t() ? tmp_interval.max() : record.get_t() };
    //                 //     hit_flag = true;
    //                 // }
    //             }
    //         }
    //         else {
    //             const bool hit_left = is_hit_(ray, interval, record, node.left_node);
    //             const bool hit_right = is_hit_(ray, Interval{ interval.min(), hit_left ? record.get_t() : interval.max() }, record, node.left_node + 1);
    //             hit_flag = hit_left || hit_right;
    //         }
    //         return hit_flag;
    //     }

    // private:
    //     std::vector<shared_ptr<Hittable>> m_primitives{};
    //     std::vector<uint> m_prim_indices{};
    //     std::vector<BVHNode> m_nodes{};
    //     uint32_t m_root_idx{ 0 };
    //     uint32_t m_node_used{ 1 };

    // };

    struct BVHNode {
        glm::vec3 aabb_min{}, aabb_max{};

        // left child node or first primitive index
        // tri_count = 0 -> internal node
        // tri_count > 0 -> leaf node
        uint32_t left_first{};
        uint32_t tri_count{};

        // cout
        std::ostream& print(std::ostream& out) const {
            out << "BVHNode: " << std::endl;
            out << "aabb_min: " << glm::to_string(aabb_min) << std::endl;
            out << "aabb_max: " << glm::to_string(aabb_max) << std::endl;
            out << "left_first: " << left_first << std::endl;
            out << "tri_count: " << tri_count << std::endl;
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const BVHNode& node) {
            return node.print(out);
        }
    };

    class BVHAccel : public Hittable {
    public:
        BVHAccel(const HittableList& obj_container) : BVHAccel{ obj_container.objects(), 0, obj_container.objects().size() } {}

        // BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range, const std::size_t& max_leaf_size = 2, std::size_t level = 0)
        BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range)
            : m_primitives{ std::vector<shared_ptr<Hittable>>(&src_objects[start],&src_objects[range]) }
            , m_prim_indices{ std::vector<uint>(m_primitives.size()) }
            , m_nodes{ std::vector<BVHNode>(2 * m_primitives.size() - 1) } {

            for (std::size_t i = 0; i < m_primitives.size(); ++i) {
                m_prim_indices[i] = i;
            }

            if (m_primitives.empty() || m_primitives.size() == 0) {
                std::cerr << "No objects in BVH constructor." << std::endl;
                return;
            }

            auto& root = m_nodes[0];
            root.left_first = 0;
            root.tri_count = m_primitives.size();

            update_node_bounds(m_root_idx);

            subdivide(m_root_idx);
        }

        void update_node_bounds(const uint node_idx) {
            BVHNode& node = m_nodes[node_idx];
            node.aabb_min = glm::vec3{ Constant::INFINITY_POS };
            node.aabb_max = glm::vec3{ Constant::INFINITY_NEG };
            for (uint i = node.left_first; i < node.left_first + node.tri_count; ++i) {
                uint prim_idx = m_prim_indices[i];
                const auto& prim = m_primitives[prim_idx];

                node.aabb_min = glm::min(node.aabb_min, prim->get_min());
                node.aabb_max = glm::max(node.aabb_max, prim->get_max());
            }
        }

        void subdivide(uint node_idx, uint depth = 0) {

            // terminate if reaches leaf node
            BVHNode& node = m_nodes[node_idx];
            if (node.tri_count <= 2) {
                // std::cout << "Depth: " << depth << "; Node idx: " << node_idx << "; Tri count: " << node.tri_count << "; at leaf node, going return //" << std::endl;
                return;
            }

            // 1. Split plane
            glm::vec3 extent = node.aabb_max - node.aabb_min;
            int axis = 0;
            if (extent.y > extent.x) { axis = 1; }
            if (extent.z > extent[axis]) { axis = 2; }

            // Find split position
            float centroid_min = Constant::INFINITY_POS;
            float centroid_max = Constant::INFINITY_NEG;
            for (uint i = node.left_first; i < node.left_first + node.tri_count; ++i) {
                uint prim_idx = m_prim_indices[i];
                const auto& prim = m_primitives[prim_idx];
                glm::vec3 centroid = prim->get_centroid();
                centroid_min = glm::min(centroid_min, centroid[axis]);
                centroid_max = glm::max(centroid_max, centroid[axis]);
            }
            float split_pos = (centroid_min + centroid_max) * 0.5f;

            // 2. Split into 2 halves
            int i = node.left_first;
            int j = node.left_first + node.tri_count - 1;
            while (i <= j) {
                const auto& prim = m_primitives[m_prim_indices[i]];
                glm::vec3 centroid = prim->get_centroid();
                if (centroid[axis] < split_pos) {
                    ++i;
                }
                else {
                    std::swap(m_prim_indices[i], m_prim_indices[j]);
                    --j;
                }
            }

            // Terminate if no split occured --> make it a leaf node
            uint left_count = i - node.left_first;
            if (left_count == 0 || left_count == node.tri_count) {
                // No split occured --> leaf node
                // std::cout << "Depth: " << depth << "; no split occured" << "; Node idx: " << node_idx << "; Tri count: " << node.tri_count << "; going return //" << std::endl;
                return;
            }

            // 3. Create child ndoes for each half
            uint left_child_idx = m_node_used++;
            uint right_child_idx = m_node_used++;
            m_nodes[left_child_idx].left_first = node.left_first;
            m_nodes[left_child_idx].tri_count = left_count;
            m_nodes[right_child_idx].left_first = i;
            m_nodes[right_child_idx].tri_count = node.tri_count - left_count;

            // Update bounds
            update_node_bounds(left_child_idx);
            update_node_bounds(right_child_idx);

            // std::cout << "Depth: " << depth << "; Bounds: " << glm::to_string(node.aabb_min) << " - " << glm::to_string(node.aabb_max) << "; count: " << node.tri_count << "; Node idx: " << node_idx;

            node.left_first = left_child_idx;
            node.tri_count = 0;

            // std::cout << "; Tri count: " << node.tri_count << "\n";

            // 4. Recursively subdivide
            // std::cout << "Depth: " << depth << " going left" << ", splitting axis: " << axis << std::endl;
            subdivide(left_child_idx, depth + 1);
            // std::cout << "Depth: " << depth << " going right" << ", splitting axis: " << axis << std::endl;
            subdivide(right_child_idx, depth + 1);

            // std::cout << "Depth: " << depth << " going up |" << std::endl;
        }

        AABB bounding_box() const override {
            return AABB{ m_nodes[m_root_idx].aabb_min, m_nodes[m_root_idx].aabb_max };
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
            return is_hit_(ray, interval, record, m_root_idx);
        }

        bool is_hit_(const Ray& ray, const Interval& interval, IntersectRecord& record, const uint node_idx) const {
            const BVHNode& node = m_nodes[node_idx];
            AABB box{ node.aabb_min, node.aabb_max };
            Interval tmp_interval{ interval };
            if (!box.is_hit(ray, tmp_interval)) {
                return false;
            }

            bool hit_flag = false;
            if (node.tri_count > 0) {
                for (uint i = 0; i < node.tri_count; ++i) {
                    const uint prim_idx = m_prim_indices[node.left_first + i];
                    const auto& prim = m_primitives[prim_idx];

                    IntersectRecord tmp_record{};
                    if (prim->is_hit(ray, interval, tmp_record)) {
                        if (tmp_record.get_t() < record.get_t()) {
                            record = tmp_record;
                            hit_flag = true;
                        }
                    }
                }
            }
            else {
                const bool hit_left = is_hit_(ray, interval, record, node.left_first);
                const bool hit_right = is_hit_(ray, Interval{ interval.min(), hit_left ? record.get_t() : interval.max() }, record, node.left_first + 1);
                hit_flag = hit_left || hit_right;
            }
            return hit_flag;
        }

    private:
        std::vector<shared_ptr<Hittable>> m_primitives{};
        std::vector<uint> m_prim_indices{};
        std::vector<BVHNode> m_nodes{};
        uint32_t m_root_idx{ 0 };
        uint32_t m_node_used{ 1 };

    };

#endif
}



#endif // !BVH_MAP_HPP
