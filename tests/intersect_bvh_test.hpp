#pragma once

#ifndef INTERSECT_BVH_TEST_HPP
#define INTERSECT_BVH_TEST_HPP

#include "intersect_test_class.hpp"
#include "intersect_record.hpp"
#include "hittable_list.hpp"
#include "ray.hpp"
#include "glm/glm.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

/*
    ----------------------------------------
    Ray-BVH Tests
    ----------------------------------------
*/
TEST_F(IntersectionTest, RayBVH1) {
    bool hit1 = bvh->is_hit(ray1, interval, record);
    EXPECT_TRUE(hit1);
    EXPECT_FLOAT_EQ(record.get_t(), 2.0f);
    EXPECT_EQ(record.get_point(), (glm::vec3{ 0.0f, 0.0f, 0.0f }));
}

TEST_F(IntersectionTest, RayBVH2) {
    bool hit2 = bvh->is_hit(ray2, interval, record);
    EXPECT_TRUE(hit2);
    EXPECT_FLOAT_EQ(record.get_t(), 1.0f);
    EXPECT_EQ(record.get_point(), (glm::vec3{ 1.0f, 0.0f, 0.0f }));
}

TEST_F(IntersectionTest, RayBVH3) {
    bool hit3 = bvh->is_hit(ray3, interval, record);
    EXPECT_FALSE(hit3);
    EXPECT_FLOAT_EQ(record.get_t(), 0.0f);
    EXPECT_EQ(record.get_point(), (glm::vec3{Constant::INFINITY_NEG}));
}


TEST_F(IntersectionTest, RayBVHHittableList1) {
    bool hit_bvh_1 = bvh->is_hit(ray1, interval, record);
    SignalTracer::IntersectRecord hittablelist_record{};
    bool hit_hittablelist_1 = model1.is_hit(ray1, interval, hittablelist_record);
    EXPECT_EQ(hit_bvh_1, hit_hittablelist_1);
    EXPECT_EQ(record, hittablelist_record);
}

TEST_F(IntersectionTest, RayBVHHittableList2) {
    bool hit_bvh_2 = bvh->is_hit(ray2, interval, record);
    SignalTracer::IntersectRecord hittablelist_record{};
    bool hit_hittablelist_2 = model1.is_hit(ray2, interval, hittablelist_record);
    EXPECT_EQ(hit_bvh_2, hit_hittablelist_2);
    EXPECT_EQ(record, hittablelist_record);
}

TEST_F(IntersectionTest, RayBVHHittableList3) {
    bool hit_bvh_3 = bvh->is_hit(ray3, interval, record);
    SignalTracer::IntersectRecord hittablelist_record{};
    bool hit_hittablelist_3 = model1.is_hit(ray3, interval, hittablelist_record);
    EXPECT_EQ(hit_bvh_3, hit_hittablelist_3);
    EXPECT_EQ(record, hittablelist_record);
}

TEST_F(IntersectionTest, EmptyBVH) {
    SignalTracer::HittableList empty_list{};
    std::shared_ptr<SignalTracer::BVH> empty_bvh = std::make_shared<SignalTracer::BVH>(empty_list);
    bool hit_empty_bvh = empty_bvh->is_hit(ray1, interval, record);
    EXPECT_FALSE(hit_empty_bvh);
}




#endif // !INTERSECT_BVH_TEST_HPP