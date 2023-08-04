#include "bvh_map.hpp"

#ifdef BVH3

namespace SignalTracer {
    BVHAccel::BVHAccel(const HittableList& obj_container) : BVHAccel{ obj_container.objects(), 0, obj_container.objects().size() } {}

    BVHAccel::BVHAccel(const std::vector<shared_ptr<Hittable>>& src_objects, const std::size_t& start, const std::size_t& range)
        : m_primitives{ std::vector<shared_ptr<Hittable>>(&src_objects[start],&src_objects[range]) }
        , m_prim_indices{ std::vector<uint>(m_primitives.size()) }
        , m_nodes{ std::vector<BVHNode>(2 * m_primitives.size()) } {
        build();
    }

    void BVHAccel::build() {
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

    void BVHAccel::subdivide(uint node_idx, uint depth) {

        BVHNode& node = m_nodes[node_idx];

        // 0. terminate if reaches leaf node
        if (node.tri_count <= 2) {
            return;
        }

        // 1. Find best split plane
        int axis{};
        float split_pos{};
        float split_cost = find_best_split_plane(node, axis, split_pos);

        AABB parent_box = AABB{ node.aabb_min, node.aabb_max };
        float parent_cost = parent_box.calc_surface_area() * node.tri_count;
        if (parent_cost < split_cost) {
            // splitting does not improve the cost --> make a leaf node
            return;
        }

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

        uint left_count = i - node.left_first;
        if (left_count == 0 || left_count == node.tri_count) {
            // No split occured --> leaf node
            return;
        }

        // 3. Create child ndoes for each half
        uint left_child_idx = m_nodes_used++;
        uint right_child_idx = m_nodes_used++;
        m_nodes[left_child_idx].left_first = node.left_first;
        m_nodes[left_child_idx].tri_count = left_count;
        m_nodes[right_child_idx].left_first = i;
        m_nodes[right_child_idx].tri_count = node.tri_count - left_count;

        // Update bounds
        update_node_bounds(left_child_idx);
        update_node_bounds(right_child_idx);
        node.left_first = left_child_idx;
        node.tri_count = 0;

        // 4. Recursively subdivide
        subdivide(left_child_idx, depth + 1);
        subdivide(right_child_idx, depth + 1);
    }

    void BVHAccel::refit() {
        for (int i = m_nodes_used - 1; i >= 0; i--) if (i != 1) {
            BVHNode& node = m_nodes[i];
            if (node.tri_count > 0) {
                // leaf node: adjust bounds to contained triangles
                update_node_bounds(i);
                continue;
            }
            // interior node: adjust bounds to child node bounds
            BVHNode& left_child = m_nodes[node.left_first];
            BVHNode& right_child = m_nodes[node.left_first + 1];
            node.aabb_min = glm::min(left_child.aabb_min, right_child.aabb_min);
            node.aabb_max = glm::max(left_child.aabb_max, right_child.aabb_max);
        }
    }

    AABB BVHAccel::bounding_box() const {
        return AABB{ m_nodes[m_root_idx].aabb_min, m_nodes[m_root_idx].aabb_max };
    }

    bool BVHAccel::is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const {
        return is_hit_(ray, interval, record);
    }

    bool BVHAccel::is_hit_(const Ray& ray, Interval interval, IntersectRecord& record) const {

        // transformed ray if BVH structure moves.
        // This is similar to the inverse transformation of the ray in the world space.
        Ray transformed_ray = ray;
        glm::mat4 invTransform = m_inv_transform;
        auto transform_origin = glm::vec3(invTransform * glm::vec4(ray.get_origin(), 1.0f));
        transformed_ray.set_origin(transform_origin);
        transformed_ray.set_direction(glm::vec3(invTransform * glm::vec4(ray.get_direction(), 0.0f)));

        // trace transformed ray
        const BVHNode* node = &m_nodes[m_root_idx], * stack[128];
        uint stack_ptr = 0;

        bool hit_flag = false;
        while (true) {
            if (node->tri_count > 0) {
                // leaf node
                for (uint i = 0; i < node->tri_count; ++i) {
                    const uint prim_idx = m_prim_indices[node->left_first + i];
                    const auto& prim = m_primitives[prim_idx];

                    IntersectRecord tmp_record{};
                    if (prim->is_hit(transformed_ray, interval, tmp_record)) {
                        if (tmp_record.get_t() < record.get_t()) {
                            record = tmp_record;
                            interval.max(record.get_t());
                            hit_flag = true;
                        }
                    }
                }
                if (stack_ptr == 0) { break; }
                else { node = stack[--stack_ptr]; }
            }
            else {
                const BVHNode* child1 = &m_nodes[node->left_first];
                const BVHNode* child2 = &m_nodes[node->left_first + 1];
                AABB left_box{ child1->aabb_min, child1->aabb_max };
                AABB right_box{ child2->aabb_min, child2->aabb_max };

                float dist1 = left_box.hit(transformed_ray, interval);
                float dist2 = right_box.hit(transformed_ray, interval);

                if (dist1 > dist2) {
                    std::swap(child1, child2);
                    std::swap(dist1, dist2);
                }
                if (dist1 == Constant::INF_POS) {
                    // no hit for both child nodes
                    if (stack_ptr == 0) break;
                    else node = stack[--stack_ptr];
                }
                else {
                    node = child1;
                    if (dist2 != Constant::INF_POS) {
                        stack[stack_ptr++] = child2;
                    }
                }
            }
        }
        return hit_flag;
    }

    void BVHAccel::set_transform(const glm::mat4& transform) {
        m_inv_transform = glm::inverse(transform);
        glm::vec3 bmin{ m_nodes[m_root_idx].aabb_min };
        glm::vec3 bmax{ m_nodes[m_root_idx].aabb_max };
        m_box = AABB{};
        for (int i = 0; i < 8; i++) {
            glm::vec4 tmp_point{ i & 1 ? bmax.x : bmin.x, i & 2 ? bmax.y : bmin.y, i & 4 ? bmax.z : bmin.z, 1.0f };
            glm::vec3 transformed_point{ transform * tmp_point };
            m_box.expand(transformed_point);
        }
    }

    void BVHAccel::update_node_bounds(const uint node_idx) {
        BVHNode& node = m_nodes[node_idx];
        node.aabb_min = glm::vec3{ Constant::INF_POS };
        node.aabb_max = glm::vec3{ Constant::INF_NEG };
        for (uint i = node.left_first; i < node.left_first + node.tri_count; ++i) {
            uint prim_idx = m_prim_indices[i];
            const auto& prim = m_primitives[prim_idx];

            node.aabb_min = glm::min(node.aabb_min, prim->get_min());
            node.aabb_max = glm::max(node.aabb_max, prim->get_max());
        }
    }

    float BVHAccel::find_best_split_plane(const BVHNode& node, int& axis, float& split_pos, uint num_bins) {
        float best_cost = Constant::INF_POS;
        glm::vec3 bound_min{ 1e30f }, bound_max{ -1e30f };
        for (uint i = 0; i < node.tri_count; i++) {
            uint prim_idx = m_prim_indices[node.left_first + i];
            const auto& prim_ptr = m_primitives[prim_idx];
            bound_min = glm::min(bound_min, prim_ptr->get_centroid());
            bound_max = glm::max(bound_max, prim_ptr->get_centroid());
        }

        for (int a = 0; a < 3; a++) {
            if (bound_min[a] == bound_max[a]) { continue; }

            // Populate bins
            Bin bins[num_bins];
            float scale = static_cast<float>(num_bins) / (bound_max[a] - bound_min[a]);
            for (uint i = 0; i < node.tri_count; i++) {
                uint prim_idx = m_prim_indices[node.left_first + i];
                const auto& prim_ptr = m_primitives[prim_idx];
                glm::vec3 centroid = prim_ptr->get_centroid();
                uint bin_idx = std::min(num_bins - 1, static_cast<uint>((centroid[a] - bound_min[a]) * scale));
                bins[bin_idx].box.expand(prim_ptr->bounding_box());
                bins[bin_idx].tri_count++;
            }

            // gather data for "num_bins-1" planes between bins
            float left_area[num_bins - 1], right_area[num_bins - 1];
            int left_count[num_bins - 1], right_count[num_bins - 1];
            AABB left_box{}, right_box{};
            int left_sum{ 0 }, right_sum{ 0 };

            for (uint i = 0; i < num_bins - 1;i++) {
                left_sum += bins[i].tri_count;
                left_count[i] = left_sum;
                left_box.expand(bins[i].box);
                left_area[i] = left_box.calc_surface_area();
                right_sum += bins[num_bins - 1 - i].tri_count;
                right_count[num_bins - 2 - i] = right_sum;
                right_box.expand(bins[num_bins - 1 - i].box);
                right_area[num_bins - 2 - i] = right_box.calc_surface_area();
            }

            // Calc SAH cost for num_bins-1 planes
            scale = (bound_max[a] - bound_min[a]) / static_cast<float>(num_bins);
            for (uint i = 0; i < num_bins - 1; i++) {
                float cost = (left_area[i] * left_count[i] + right_area[i] * right_count[i]);
                if (cost < best_cost) {
                    split_pos = bound_min[a] + scale * (i + 1);
                    best_cost = cost;
                    axis = a;
                }
            }
        }
        return best_cost;
    }

    /*
        No Binning
    */
    // float find_best_split_plane(const BVHNode& node, int& axis, float& split_pos) {
        //     float best_cost = Constant::INF_POS;
        //     glm::vec3 bound_min{ 1e30f }, bound_max{ -1e30f };
        //     for (uint i = 0; i < node.tri_count; i++) {
        //         uint prim_idx = m_prim_indices[node.left_first + i];
        //         const auto& prim_ptr = m_primitives[prim_idx];
        //         bound_min = glm::min(bound_min, prim_ptr->get_centroid());
        //         bound_max = glm::max(bound_max, prim_ptr->get_centroid());
        //     }

        //     uint num_bins = 100;
        //     for (int a = 0; a < 3; a++) {
        //         if (bound_min[a] == bound_max[a]) { continue; }
        //         float scale = (bound_max[a] - bound_min[a]) / static_cast<float>(num_bins);

        //         for (uint i = 1; i < num_bins; i++) {
        //             float candidate_pos = bound_min[a] + scale * i;
        //             float cost = evaluate_sah(node, a, candidate_pos);
        //             if (cost < best_cost) {
        //                 split_pos = candidate_pos;
        //                 best_cost = cost;
        //                 axis = a;
        //             }
        //         }
        //     }
        //     return best_cost;
        // }

        // float evaluate_sah(const BVHNode& node, int axis, float pos) {
        //     AABB left_box{}, right_box{};
        //     uint left_count{ 0 }, right_count{ 0 };
        //     for (uint i = 0; i < node.tri_count; ++i) {
        //         uint prim_idx = m_prim_indices[node.left_first + i];
        //         const auto& prim_ptr = m_primitives[prim_idx];
        //         if (prim_ptr->get_centroid()[axis] < pos) {
        //             left_count++;
        //             left_box.expand(prim_ptr->bounding_box());
        //         }
        //         else {
        //             right_count++;
        //             right_box.expand(prim_ptr->bounding_box());
        //         }
        //     }
        //     float cost = left_count * left_box.calc_surface_area() + right_count * right_box.calc_surface_area();
        //     return cost > 0 ? cost : Constant::INF_POS;
        // }

    TLAS::TLAS(BVHAccel* bvh_list, uint num_bvh)
        : m_blas{ bvh_list }, m_blas_count{ num_bvh } {
        m_tlas_node = (TLASNode*) aligned_alloc(sizeof(TLASNode) * 2 * num_bvh, 64);
        m_nodes_used = 2;
    }

    TLAS::~TLAS() {
        free(m_tlas_node);
    }

    void TLAS::build() {
        // for (uint i = 0; i < m_blas_count; ++i) {
        //     m_tlas_node[i].left_first = i;
        //     m_tlas_node[i].tri_count = 1;
        //     m_tlas_node[i].aabb_min = m_blas[i].bounding_box().get_min();
        //     m_tlas_node[i].aabb_max = m_blas[i].bounding_box().get_max();
        // }
        // m_root_idx = 0;
        // subdivide(m_root_idx);
    }

    bool TLAS::is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const {
        return is_hit_(ray, interval, record);
    }

    bool TLAS::is_hit_(const Ray& ray, Interval interval, IntersectRecord& record) const {
        // trace transformed ray
        const TLASNode* node = &m_tlas_node[0], * stack[128];
        uint stack_ptr = 0;

        bool hit_flag = false;
        while (true) {
            if (node->is_leaf != 0) {
                // tlas leaf node
                IntersectRecord tmp_record{};
                if (m_blas[node->left_blas].is_hit(ray, interval, tmp_record)) {
                    if (tmp_record.get_t() < record.get_t()) {
                        record = tmp_record;
                        interval.max(record.get_t());
                        hit_flag = true;
                    }
                }
                if (stack_ptr == 0) { break; }
                else { node = stack[--stack_ptr]; }
            }
            else {
                const TLASNode* child1 = &m_tlas_node[node->left_blas];
                const TLASNode* child2 = &m_tlas_node[node->left_blas + 1];
                AABB left_box{ child1->aabb_min, child1->aabb_max };
                AABB right_box{ child2->aabb_min, child2->aabb_max };

                float dist1 = left_box.hit(ray, interval);
                float dist2 = right_box.hit(ray, interval);

                if (dist1 > dist2) {
                    std::swap(child1, child2);
                    std::swap(dist1, dist2);
                }
                if (dist1 == Constant::INF_POS) {
                    // no hit for both child nodes
                    if (stack_ptr == 0) break;
                    else node = stack[--stack_ptr];
                }
                else {
                    node = child1;
                    if (dist2 != Constant::INF_POS) {
                        stack[stack_ptr++] = child2;
                    }
                }
            }
        }
        return hit_flag;
    }
}



#endif