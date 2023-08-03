#pragma once

#ifndef INTERSECT_HITTABLELIST_TEST_HPP
#define INTERSECT_HITTABLELIST_TEST_HPP

#include "constant.hpp"
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
    Ray-HittableList Tests
    ----------------------------------------
*/
TEST_F(IntersectionTest, RayHittableList1) {
    bool hit1 = model1.is_hit(ray1, interval, record);
    EXPECT_TRUE(hit1);
    EXPECT_FLOAT_EQ(record.get_t(), 2.0f);
    EXPECT_EQ(record.get_point(), (glm::vec3{ 0.0f, 0.0f, 0.0f }));
}

TEST_F(IntersectionTest, RayHittableList2) {
    bool hit2 = model1.is_hit(ray2, interval, record);
    EXPECT_TRUE(hit2);
    EXPECT_FLOAT_EQ(record.get_t(), 1.0f);
    EXPECT_EQ(record.get_point(), (glm::vec3{ 1.0f, 0.0f, 0.0f }));
}

TEST_F(IntersectionTest, RayHittableList3) {
    bool hit3 = model1.is_hit(ray3, interval, record);
    EXPECT_FALSE(hit3);
    EXPECT_FLOAT_EQ(record.get_t(), 0.0f);
    EXPECT_EQ(record.get_point(), (glm::vec3{Constant::INF_NEG}));

}

#endif // INTERSECT_HITTABLELIST_TEST_HPP