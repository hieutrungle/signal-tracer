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
    glm::vec3 origin{ 1.0f, 2.0f, 3.0f };
    glm::vec3 direction{ 4.0f, 5.0f, 6.0f };
    SignalTracer::Ray ray{ origin, direction };
    EXPECT_EQ(ray.get_origin(), origin);
    EXPECT_EQ(ray.get_direction(), direction);
}

TEST(RayTest, RayCopyConstructor) {
    glm::vec3 origin{ 1.0f, 2.0f, 3.0f };
    glm::vec3 direction{ 4.0f, 5.0f, 6.0f };
    SignalTracer::Ray ray1{ origin, direction };
    SignalTracer::Ray ray2{ ray1 };
    EXPECT_EQ(ray1.get_origin(), ray2.get_origin());
    EXPECT_EQ(ray1.get_direction(), ray2.get_direction());
}

TEST(RayTest, RayMoveConstructor) {
    glm::vec3 origin{ 1.0f, 2.0f, 3.0f };
    glm::vec3 direction{ 4.0f, 5.0f, 6.0f };
    SignalTracer::Ray ray1{ origin, direction };
    SignalTracer::Ray ray2{ std::move(ray1) };
    EXPECT_EQ(ray2.get_origin(), origin);
    EXPECT_EQ(ray2.get_direction(), direction);
}

TEST(RayTest, RayAssignmentOperator) {
    glm::vec3 origin{ 1.0f, 2.0f, 3.0f };
    glm::vec3 direction{ 4.0f, 5.0f, 6.0f };
    SignalTracer::Ray ray1{ origin, direction };
    SignalTracer::Ray ray2{};
    ray2 = ray1;
    EXPECT_EQ(ray2.get_origin(), origin);
    EXPECT_EQ(ray2.get_direction(), direction);
}

TEST(RayTest, RayMoveAssignmentOperator) {
    glm::vec3 origin{ 1.0f, 2.0f, 3.0f };
    glm::vec3 direction{ 4.0f, 5.0f, 6.0f };
    SignalTracer::Ray ray1{ origin, direction };
    SignalTracer::Ray ray2{};
    ray2 = std::move(ray1);
    EXPECT_EQ(ray2.get_origin(), origin);
    EXPECT_EQ(ray2.get_direction(), direction);
}


#endif // !RAY_TEST_HPP