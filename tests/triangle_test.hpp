#pragma once

#ifndef TRIANGLE_TEST_HPP
#define TRIANGLE_TEST_HPP

#include "aabb.hpp"
#include "utils.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

/*
    ----------------------------------------
    Triangle Tests
    ----------------------------------------
*/

TEST(TriangleTest, Constructor) {
    glm::vec3 p1{0.0f, 0.0f, 0.0f};
    glm::vec3 p2{1.0f, 0.0f, 0.0f};
    glm::vec3 p3{0.0f, 1.0f, 0.0f};
    std::shared_ptr<signal_tracer::Material> p_material = std::make_shared<signal_tracer::Material>();
    signal_tracer::Triangle triangle(p1, p2, p3, p_material);
    EXPECT_EQ(triangle[0], p1);
    EXPECT_EQ(triangle[1], p2);
    EXPECT_EQ(triangle[2], p3);
    EXPECT_EQ(triangle.p_material(), p_material);
    EXPECT_EQ(triangle.bounding_box(), (signal_tracer::AABB(p1, p2, p3)));
}

TEST(TriangleTest, ChangeVertices) {
    glm::vec3 p1{0.0f, 0.0f, 0.0f};
    glm::vec3 p2{1.0f, 0.0f, 0.0f};
    glm::vec3 p3{0.0f, 1.0f, 0.0f};
    glm::vec3 p4{-1.0f, 4.0f, -7.0f};
    signal_tracer::Triangle triangle(p1, p2, p3);
    triangle.a(p4);
    EXPECT_EQ(triangle[0], p4);
    EXPECT_EQ(triangle[1], p2);
    EXPECT_EQ(triangle[2], p3);
    EXPECT_EQ(triangle.bounding_box(), (signal_tracer::AABB(p2, p3, p4)));
}

TEST(TriangleTest, MoveConstructor) {
    glm::vec3 p1{0.0f, 0.0f, 0.0f};
    glm::vec3 p2{1.0f, 0.0f, 0.0f};
    glm::vec3 p3{0.0f, 1.0f, 0.0f};
    glm::vec3 p4{-1.0f, 4.0f, -7.0f};
    signal_tracer::Triangle triangle1(p1, p2, p3);
    signal_tracer::Triangle triangle2(std::move(triangle1));
    EXPECT_TRUE(triangle2 == (signal_tracer::Triangle(p1, p2, p3)));
}

TEST(TriangleTest, TwoTriangles) {
    glm::vec3 p1{0.0f, 0.0f, 0.0f};
    glm::vec3 p2{1.0f, 0.0f, 0.0f};
    glm::vec3 p3{0.0f, 1.0f, 0.0f};
    glm::vec3 p4{-1.0f, 4.0f, -7.0f};
    signal_tracer::Triangle triangle1(p1, p2, p3);
    signal_tracer::Triangle triangle2(p1, p2, p4);
    EXPECT_TRUE(triangle1 == (signal_tracer::Triangle(p1, p2, p3)));
    EXPECT_TRUE(triangle2 == (signal_tracer::Triangle(p1, p2, p4)));
    EXPECT_TRUE(triangle1 != triangle2);
}

TEST(TriangleTest, CompareTwoTriangles) {
    glm::vec3 p1{0.0f, 0.0f, 0.0f};
    glm::vec3 p2{1.0f, 0.0f, 0.0f};
    glm::vec3 p3{0.0f, 1.0f, 0.0f};
    glm::vec3 p4{-1.0f, 4.0f, -7.0f};
    signal_tracer::Triangle triangle1(p1, p2, p3);
    signal_tracer::Triangle triangle2(p1, p2, p4);
    EXPECT_TRUE(triangle1 == (signal_tracer::Triangle(p1, p2, p3)));
    EXPECT_TRUE(triangle1 == (signal_tracer::Triangle(p2, p3, p1)));
    EXPECT_TRUE(triangle2 == (signal_tracer::Triangle(p1, p2, p4)));
    EXPECT_TRUE(triangle2 == (signal_tracer::Triangle(p4, p1, p2)));
    EXPECT_TRUE(triangle2 != (signal_tracer::Triangle(p4, p3, p2)));
    EXPECT_TRUE(triangle1 != triangle2);
}

#endif // !TRIANGLE_TEST_HPP