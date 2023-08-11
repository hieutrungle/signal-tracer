#pragma once

#ifndef _BVH_MAP_HPP_
#define _BVH_MAP_HPP_

#include "triangle.hpp"
#include "constant.hpp"
#include "hittable_list.hpp"
#include "model.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "omp.h"
#include <mutex>
#include <thread>
#include <algorithm>
#include <vector>


namespace SignalTracer {

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

    private:

        std::vector<std::shared_ptr<Hittable>> init_triangles(const Model& model);
        void update_node_bounds(const uint node_idx);
        float find_best_split_plane(const BVHNode& node, int& axis, float& split_pos, uint num_bins = 64);
        void subdivide(uint node_idx, uint depth = 0);

        std::vector<shared_ptr<Hittable>> m_primitives{};
        std::vector<uint> m_prim_indices{};
        std::vector<BVHNode> m_nodes{};
        uint32_t m_nodes_used{ 2 };
    };

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
