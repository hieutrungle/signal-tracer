#pragma once

#ifndef BVH_HPP
#define BVH_HPP

#include "triangle.hpp"
#include "constant.hpp"
#include "hittable_list.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>
#include <vector>

namespace SignalTracer {

    // class BVHNode {
    // public:

    //     BVHNode(const AABB& box, std::size_t start, std::size_t range)
    //         : m_box{ box }, m_start{ start }, m_range{ range }, m_left{ nullptr }, m_right{ nullptr } {}

    //     inline bool is_leaf() const { return m_left == nullptr && m_right == nullptr; }

    //     AABB m_box{}; ///< bounding box ofthe node
    //     std::size_t m_start{}; ///< start index into the primitive list
    //     std::size_t m_range{}; ///< range of index into the primitive list
    //     std::shared_ptr<BVHNode> m_left{}; ///< left child node
    //     std::shared_ptr<BVHNode> m_right{}; ///< right child node
    // };

    // class BVHAccel : public Hittable {
    // public:
    //     BVHAccel() = default;

    //     /**
    //      * Parameterized Constructor.
    //      * Create BVH from a list of primitives. Note that the BVHAccel Aggregate
    //      * stores pointers to the primitives and thus the primitives need be kept
    //      * in memory for the aggregate to function properly.
    //      * \param primitives primitives to build from
    //      * \param max_leaf_size maximum number of primitives to be stored in leaves
    //      */
    //     BVHAccel(const std::vector<Hittable>& primitives, int max_leaf_size = 4);

    //     /**
    //      * Destructor.
    //      * The destructor only destroys the Aggregate itself, the primitives that
    //      * it contains are left untouched.
    //      */
    //     ~BVHAccel();

    //     /**
    //      * Get the world space bounding box of the aggregate.
    //      * \return world space bounding box of the aggregate
    //      */
    //     AABB get_bbox() const;

    //     bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;

    //     /**
    //      * Ray - Aggregate intersection.
    //      * Check if the given ray intersects with the aggregate (any primitive in
    //      * the aggregate), no intersection information is stored.
    //      * \param r ray to test intersection with
    //      * \return true if the given ray intersects with the aggregate,
    //                false otherwise
    //      */
    //      // bool intersect(const Ray& r) const;

    //      /**
    //       * Ray - Aggregate intersection 2.
    //       * Check if the given ray intersects with the aggregate (any primitive in
    //       * the aggregate). If so, the input intersection data is updated to contain
    //       * intersection information for the point of intersection. Note that the
    //       * intersected primitive entry in the intersection should be updated to
    //       * the actual primitive in the aggregate that the ray intersected with and
    //       * not the aggregate itself.
    //       * \param r ray to test intersection with
    //       * \param i address to store intersection info
    //       * \return true if the given ray intersects with the aggregate,
    //                 false otherwise
    //       */
    //       // bool intersect(const Ray& r, Intersection* i) const;

    //       /**
    //        * Get BSDF of the surface material
    //        * Note that this does not make sense for the BVHAccel aggregate
    //        * because it does not have a surface material. Therefore this
    //        * should always return a null pointer.
    //        */
    //        // BSDF* get_bsdf() const { return NULL; }

    //        /**
    //         * Get entry point (root) - used in visualizer
    //         */
    //     std::shared_ptr<BVHNode> get_root() const { return m_root; }

    //     /**
    //      * Draw the BVH with OpenGL - used in visualizer
    //      */
    //      // void draw(const Color& c) const {}

    //      /**
    //       * Draw the BVH outline with OpenGL - used in visualizer
    //       */
    //       // void drawOutline(const Color& c) const {}

    //     void partition(BVHNode& node, int max_leaf_size);
    //     static inline int compute_bucket(double centroid_min, double centroid_max, double centroid);

    // private:
    //     // void find_closest_hit(const Ray& ray, const BVHNode& node, Intersection& i) const;

    // private:
    //     std::shared_ptr<BVHNode> m_root{ nullptr };

    // };
}

#endif // !BVH_HPP