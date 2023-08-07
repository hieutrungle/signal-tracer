#pragma once

#ifndef _BVH_MAP_HPP_
#define _BVH_MAP_HPP_

#include "triangle.hpp"
#include "constant.hpp"
#include "hittable_list.hpp"
#include "model.hpp"
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

    struct Bin {
        AABB box{};
        uint tri_count{ 0 };
    };

    struct BVHNode {

        // left child node or first primitive index
        // tri_count = 0 -> internal node
        // tri_count > 0 -> leaf node

        // union {
        //     struct { glm::vec3 aabb_min; uint32_t left_first; };
        //     __m128 aabb_min4_sse;
        // };
        // union {
        //     struct { glm::vec3 aabb_max; uint32_t tri_count; };
        //     __m128 aabb_max4_sse;
        // };

        glm::vec3 aabb_min{};
        uint32_t left_first{};
        glm::vec3 aabb_max{};
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
        BVHAccel(const HittableList& obj_container);
        BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range);
        BVHAccel(const Model& model);

        const BVHNode& get_node(const uint node_idx) const { return m_nodes[node_idx]; }
        const BVHNode& get_root() const { return m_nodes[0]; }

        void build();
        void refit();

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;
        bool is_hit_(const Ray& ray, Interval interval, IntersectRecord& record) const;

        AABB bounding_box() const override;
        // void set_transform(const glm::mat4& transform);

    private:

        std::vector<std::shared_ptr<Hittable>> init_triangles(const Model& model);
        void update_node_bounds(const uint node_idx);
        float find_best_split_plane(const BVHNode& node, int& axis, float& split_pos, uint num_bins = 64);
        void subdivide(uint node_idx, uint depth = 0);

        std::vector<shared_ptr<Hittable>> m_primitives{};
        std::vector<uint> m_prim_indices{};
        std::vector<BVHNode> m_nodes{};
        uint32_t m_nodes_used{ 2 };
        glm::mat4 m_inv_transform{ 1.0 };
    };

#endif

    class BVHInstance : public Hittable {
    public:
        BVHInstance() = default;
        BVHInstance(const std::shared_ptr<BVHAccel>& bvh_ptr) : m_bvh_ptr{ bvh_ptr } {
            set_transform(glm::mat4{ 1.0f });
        }
        BVHInstance(const std::shared_ptr<BVHAccel>& bvh_ptr, const glm::mat4& transform) : m_bvh_ptr{ bvh_ptr } {
            set_transform(transform);
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;

        AABB bounding_box() const override;

        void set_transform(const glm::mat4& transform);

    private:
        std::shared_ptr<BVHAccel> m_bvh_ptr{ nullptr };
        glm::mat4 m_transform_point{ 1.0f };
        glm::mat4 m_transform_vector{ 1.0f };
        glm::mat4 m_inv_transform_point{ 1.0f };
        glm::mat4 m_inv_transform_vector{ 1.0f };
        AABB m_box{}; // in world space
    };

    struct TLASNode {
        glm::vec3 aabb_min{};
        uint32_t left_right{}; // 2x16 bits
        glm::vec3 aabb_max{};
        uint32_t blas_idx{};

        // child nodes of internal nodes must be greater than 0
        // leaf nodes must be 0
        bool is_leaf() { return left_right == 0; }
        bool is_leaf() const { return left_right == 0; }
    };

    class TLAS : public Hittable {
    public:
        TLAS() = default;
        TLAS(BVHInstance* bvh_list, uint blas_count);
        TLAS(const std::vector<BVHInstance>& bvh_list, uint blas_count);

        void build();
        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;
        bool is_hit_(const Ray& ray, Interval interval, IntersectRecord& record) const;


        AABB bounding_box() const override {
            return AABB{ m_tlas_nodes[0].aabb_min, m_tlas_nodes[0].aabb_max };
        }

    private:
        int find_best_match(int* node_idxs, int num_nodes, int node_A_idx);

        std::vector<TLASNode> m_tlas_nodes{};
        const BVHInstance* m_blas{ nullptr };
        uint m_blas_count{ 0 };
        uint m_nodes_used{ 0 };
    };

}

#endif // !_BVH_MAP_HPP_
