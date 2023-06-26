#pragma once

#ifndef RAY_TEST_HPP
#define RAY_TEST_HPP

#include "ray.hpp"
#include <gtest/gtest.h>

/*
    ----------------------------------------
    Ray Tests
    ----------------------------------------
*/
TEST(RayTest, RayConstructor) {
    glm::vec3 origin{1.0f, 2.0f, 3.0f};
    glm::vec3 direction{4.0f, 5.0f, 6.0f};
    signal_tracer::Ray ray{origin, direction};
    EXPECT_EQ(ray.origin(), origin);
    EXPECT_EQ(ray.direction(), direction);
}

TEST(RayTest, RayCopyConstructor) {
    glm::vec3 origin{1.0f, 2.0f, 3.0f};
    glm::vec3 direction{4.0f, 5.0f, 6.0f};
    signal_tracer::Ray ray1{origin, direction};
    signal_tracer::Ray ray2{ray1};
    EXPECT_EQ(ray1.origin(), ray2.origin());
    EXPECT_EQ(ray1.direction(), ray2.direction());
}

TEST(RayTest, RayMoveConstructor) {
    glm::vec3 origin{1.0f, 2.0f, 3.0f};
    glm::vec3 direction{4.0f, 5.0f, 6.0f};
    signal_tracer::Ray ray1{origin, direction};
    signal_tracer::Ray ray2{std::move(ray1)};
    EXPECT_EQ(ray2.origin(), origin);
    EXPECT_EQ(ray2.direction(), direction);
}

TEST(RayTest, RayAssignmentOperator) {
    glm::vec3 origin{1.0f, 2.0f, 3.0f};
    glm::vec3 direction{4.0f, 5.0f, 6.0f};
    signal_tracer::Ray ray1{origin, direction};
    signal_tracer::Ray ray2{};
    ray2 = ray1;
    EXPECT_EQ(ray2.origin(), origin);
    EXPECT_EQ(ray2.direction(), direction);
}

TEST(RayTest, RayMoveAssignmentOperator) {
    glm::vec3 origin{1.0f, 2.0f, 3.0f};
    glm::vec3 direction{4.0f, 5.0f, 6.0f};
    signal_tracer::Ray ray1{origin, direction};
    signal_tracer::Ray ray2{};
    ray2 = std::move(ray1);
    EXPECT_EQ(ray2.origin(), origin);
    EXPECT_EQ(ray2.direction(), direction);
}


#endif // !RAY_TEST_HPP