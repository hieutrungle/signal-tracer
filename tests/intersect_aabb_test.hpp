#pragma once

#ifndef INTERSECTION_AABB_TEST_HPP
#define INTERSECTION_AABB_TEST_HPP

#include "aabb.hpp"
#include "intersect_test_class.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include "ray.hpp"
#include <gtest/gtest.h>

/*
    ----------------------------------------
    Ray-AABB Tests
    ----------------------------------------
*/

TEST_F(IntersectionTest, RayAABB1) {

    signal_tracer::Triangle triangle(p1, p2, p3, p_material);
    signal_tracer::Ray ray11{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};
    signal_tracer::Interval interval1{0.1f, 100.0f};
    bool hit11 = triangle.bounding_box().intersect(ray11, interval1);
    EXPECT_TRUE(hit11);
    if (hit11) {
        bool hit12 = triangle.is_hit(ray11, interval1, record);
        EXPECT_TRUE(hit12);
    }

    signal_tracer::Interval interval2{0.1f, 100.0f};
    signal_tracer::Ray ray22{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f}};
    bool hit21 = triangle.bounding_box().intersect(ray22, interval2);
    EXPECT_FALSE(hit21);
    if (hit21) {
        bool hit22 = triangle.is_hit(ray22, interval2, record);
        EXPECT_FALSE(hit22);
    }

    signal_tracer::Interval interval3{0.1f, 100.0f};
    signal_tracer::Ray ray33{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};
    bool hit31 = triangle.bounding_box().intersect(ray33, interval3);
    EXPECT_TRUE(hit31);
    if (hit31) {
        bool hit32 = triangle.is_hit(ray33, interval3, record);
        EXPECT_TRUE(hit32);
    }

    signal_tracer::Interval interval4{0.1f, 100.0f};
    signal_tracer::Ray ray44{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.1f, -0.1f, -1.0f}};
    bool hit41 = triangle.bounding_box().intersect(ray44, interval4);
    EXPECT_TRUE(hit41);
    if (hit41) {
        bool hit42 = triangle.is_hit(ray44, interval4, record);
        EXPECT_TRUE(hit42);
    }

    signal_tracer::Interval interval5{0.1f, 100.0f};
    signal_tracer::Ray ray55{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.01f, -0.01f, -1.0f}};
    bool hit51 = triangle.bounding_box().intersect(ray55, interval5);
    EXPECT_TRUE(hit51);
    if (hit51) {
        bool hit52 = triangle.is_hit(ray55, interval5, record);
        EXPECT_TRUE(hit52);
    }

    signal_tracer::Interval interval6{0.1f, 100.0f};
    signal_tracer::Ray ray66{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.5f, -0.5f, -1.0f}};
    bool hit61 = triangle.bounding_box().intersect(ray66, interval6);
    EXPECT_TRUE(hit61);
    if (hit61) {
        bool hit62 = triangle.is_hit(ray66, interval6, record);
        EXPECT_TRUE(hit62);
    }

    signal_tracer::Interval interval7{0.1f, 100.0f};
    signal_tracer::Ray ray77{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.6f, -0.6f, -1.0f}};
    bool hit71 = triangle.bounding_box().intersect(ray77, interval7);
    EXPECT_FALSE(hit71);
    if (hit71) {
        bool hit72 = triangle.is_hit(ray77, interval7, record);
        EXPECT_FALSE(hit72);
    }

    signal_tracer::Interval interval8{1.1f, 100.0f};
    signal_tracer::Ray ray88{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.4f, -0.4f, -1.0f}};
    bool hit81 = triangle.bounding_box().intersect(ray88, interval8);
    EXPECT_FALSE(hit81);
    if (hit81) {
        bool hit82 = triangle.is_hit(ray88, interval8, record);
        EXPECT_FALSE(hit82);
    }

    signal_tracer::Interval interval99{0.1f, 0.9f};
    signal_tracer::Ray ray9{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.4f, -0.4f, -1.0f}};
    bool hit91 = triangle.bounding_box().intersect(ray9, interval99);
    EXPECT_FALSE(hit91);
    if (hit91) {
        bool hit92 = triangle.is_hit(ray9, interval99, record);
        EXPECT_FALSE(hit92);
    }

    signal_tracer::Interval interval1010{0.1f, 1.0f};
    signal_tracer::Ray ray10{glm::vec3{0.5f, 0.5f, 1.0f}, glm::vec3{-0.4f, -0.4f, -1.0f}};
    bool hit101 = triangle.bounding_box().intersect(ray10, interval1010);
    EXPECT_TRUE(hit101);
    if (hit101) {
        bool hit102 = triangle.is_hit(ray10, interval1010, record);
        EXPECT_TRUE(hit102);
    }
}

TEST_F(IntersectionTest, RayAABB2) {
    signal_tracer::Triangle triangle(p4, p2, p3, p_material);
    signal_tracer::Ray ray{glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}};
    bool hit1 = triangle.bounding_box().intersect(ray, interval);
    if (hit1) {
        bool hit2 = triangle.is_hit(ray, interval, record);
        EXPECT_FALSE(hit2);
    }
    else {
        EXPECT_TRUE(false);
    }
}

#endif // !INTERSECTION_AABB_TEST_HPP