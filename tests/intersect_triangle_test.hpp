#pragma once

#ifndef INTERSECTION_TRIANGLE_TEST_HPP
#define INTERSECTION_TRIANGLE_TEST_HPP

#include "intersect_test_class.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include "ray.hpp"
#include <gtest/gtest.h>


/*
    ----------------------------------------
    Ray-Triangle Tests
    ----------------------------------------
*/

TEST_F(IntersectionTest, RayTriangle1) {
    signal_tracer::Triangle triangle(p1, p2, p3, p_material);
    signal_tracer::Ray ray11{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};
    signal_tracer::Ray ray12{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f}};
    signal_tracer::Ray ray13{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}};
    signal_tracer::Ray ray14{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}};
    signal_tracer::Ray ray15{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f}};
    signal_tracer::Ray ray16{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}};
    signal_tracer::Ray ray17{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{1.0f, 1.0f, 0.0f}};
    signal_tracer::Ray ray18{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};

    bool hit11 = triangle.intersect(ray11, interval, record);
    bool hit12 = triangle.intersect(ray12, interval, record);
    bool hit13 = triangle.intersect(ray13, interval, record);
    bool hit14 = triangle.intersect(ray14, interval, record);
    bool hit15 = triangle.intersect(ray15, interval, record);
    bool hit16 = triangle.intersect(ray16, interval, record);
    bool hit17 = triangle.intersect(ray17, interval, record);
    bool hit18 = triangle.intersect(ray18, interval, record);

    EXPECT_TRUE(hit11);
    EXPECT_FALSE(hit12);
    EXPECT_FALSE(hit13);
    EXPECT_FALSE(hit14);
    EXPECT_FALSE(hit15);
    EXPECT_FALSE(hit16);
    EXPECT_FALSE(hit17);
    EXPECT_TRUE(hit18);
}

TEST_F(IntersectionTest, RayTriangle2) {
    signal_tracer::Ray ray21{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};
    signal_tracer::Ray ray22{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f}};
    signal_tracer::Ray ray23{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}};
    signal_tracer::Ray ray24{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}};
    signal_tracer::Ray ray25{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f}};
    signal_tracer::Ray ray26{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}};
    signal_tracer::Triangle triangle(p1, p2, p4, p_material);
    bool hit21 = triangle.intersect(ray21, interval, record);
    bool hit22 = triangle.intersect(ray22, interval, record);
    bool hit23 = triangle.intersect(ray23, interval, record);
    bool hit24 = triangle.intersect(ray24, interval, record);
    bool hit25 = triangle.intersect(ray25, interval, record);
    bool hit26 = triangle.intersect(ray26, interval, record);
    EXPECT_TRUE(hit21);
    EXPECT_FALSE(hit22);
    EXPECT_FALSE(hit23);
    EXPECT_FALSE(hit24);
    EXPECT_FALSE(hit25);
    EXPECT_FALSE(hit26);
}

TEST_F(IntersectionTest, RayTriangle3ShortInterval) {
    interval = signal_tracer::Interval{ 5.0f, 6.0f };
    signal_tracer::Triangle triangle(p1, p2, p3, p_material);
    signal_tracer::Ray ray11{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};
    signal_tracer::Ray ray22{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f}};
    signal_tracer::Ray ray33{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f}};
    signal_tracer::Ray ray44{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}};
    signal_tracer::Ray ray55{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f}};
    signal_tracer::Ray ray66{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}};
    signal_tracer::Ray ray77{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{1.0f, 1.0f, 0.0f}};
    signal_tracer::Ray ray88{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};

    bool hit11 = triangle.intersect(ray11, interval, record);
    bool hit12 = triangle.intersect(ray22, interval, record);
    bool hit13 = triangle.intersect(ray33, interval, record);
    bool hit14 = triangle.intersect(ray44, interval, record);
    bool hit15 = triangle.intersect(ray55, interval, record);
    bool hit16 = triangle.intersect(ray66, interval, record);
    bool hit17 = triangle.intersect(ray77, interval, record);
    bool hit18 = triangle.intersect(ray88, interval, record);

    EXPECT_FALSE(hit11);
    EXPECT_FALSE(hit12);
    EXPECT_FALSE(hit13);
    EXPECT_FALSE(hit14);
    EXPECT_FALSE(hit15);
    EXPECT_FALSE(hit16);
    EXPECT_FALSE(hit17);
    EXPECT_FALSE(hit18);
}

#endif // !INTERSECTION_TRIANGLE_TEST_HPP