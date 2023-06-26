#pragma once

#ifndef AABB_TEST_HPP
#define AABB_TEST_HPP

#include "aabb.hpp"
#include <gtest/gtest.h>
#include <iostream>

/*
    ----------------------------------------
    AABB Tests
    ----------------------------------------
*/

TEST(AABBTest, AABBUninitilization) {
    signal_tracer::AABB aabb{};
    // EXPECT_EQ(aabb.length(), (glm::vec3{0.0f, 0.0f, 0.0f}));
    EXPECT_FALSE(aabb.contains(glm::vec3{0.0f, 0.0f, 0.0f}));
    EXPECT_FALSE(aabb.surrounds(glm::vec3{0.0f, 0.0f, 0.0f}));
}

TEST(AABBTest, AABBConstructorOrdered) {

    glm::vec3 p1{1.0f, 2.0f, 3.0f};
    glm::vec3 p2{4.0f, 5.0f, 6.0f};
    signal_tracer::AABB aabb{p1, p2};
    EXPECT_EQ(aabb.length(), (glm::vec3{3.0f, 3.0f, 3.0f}));
    EXPECT_TRUE(aabb.contains(p1));
    EXPECT_TRUE(aabb.contains(p2));
    EXPECT_TRUE(aabb.contains(glm::vec3{2.5f, 3.5f, 4.5f}));
    EXPECT_TRUE(aabb.surrounds(glm::vec3{2.5f, 3.5f, 4.5f}));
    EXPECT_TRUE(aabb == (signal_tracer::AABB {p1, p2}));
}

TEST(AABBTest, AABBTest_AABBConstructorUnoder) {
    glm::vec3 p1{4.0f, 5.0f, 3.0f};
    glm::vec3 p2{1.0f, 2.0f, 6.0f};
    signal_tracer::AABB aabb{p1, p2};
    EXPECT_EQ(aabb.length(), (glm::vec3{std::fabs(p1.x - p2.x), std::fabs(p1.y - p2.y), std::fabs(p1.z - p2.z)}));
    EXPECT_TRUE(aabb == (signal_tracer::AABB {p1, p2}));
    EXPECT_TRUE(aabb.contains(p1));
    EXPECT_TRUE(aabb.contains(p2));
    EXPECT_TRUE(aabb.contains(glm::vec3{2.5f, 3.5f, 4.5f}));
    EXPECT_TRUE(aabb.surrounds(glm::vec3{2.5f, 3.5f, 4.5f}));
}

TEST(AABBTest, AABBContaining) {
    glm::vec3 p1{1.0f, 2.0f, 3.0f};
    glm::vec3 p2{4.0f, 5.0f, 6.0f};
    signal_tracer::AABB aabb{p1, p2};
    EXPECT_TRUE(aabb.contains(p1));
    EXPECT_TRUE(aabb.contains(p2));
    EXPECT_TRUE(aabb.contains(glm::vec3{2.5f, 3.5f, 4.5f}));
    EXPECT_FALSE(aabb.contains(glm::vec3{0.5f, 0.5f, 0.5f}));
    EXPECT_FALSE(aabb.contains(glm::vec3{7.5f, 7.5f, 7.5f}));
}

TEST(AABBTest, AABBSurrounding) {
    glm::vec3 p1{1.0f, 2.0f, 3.0f};
    glm::vec3 p2{4.0f, 5.0f, 6.0f};
    signal_tracer::AABB aabb{p1, p2};
    EXPECT_FALSE(aabb.surrounds(p1));
    EXPECT_FALSE(aabb.surrounds(p2));
    EXPECT_TRUE(aabb.surrounds(glm::vec3{2.5f, 3.5f, 4.5f}));
    EXPECT_FALSE(aabb.surrounds(glm::vec3{0.5f, 0.5f, 0.5f}));
    EXPECT_FALSE(aabb.surrounds(glm::vec3{7.5f, 7.5f, 7.5f}));
}


TEST(AABBTest, AABBCombinationTwoAABBs) {
    glm::vec3 p1{1.0f, 5.0f, 3.0f};
    glm::vec3 p2{4.0f, 2.0f, 6.0f};
    signal_tracer::AABB aabb{p1, p2};
    signal_tracer::AABB aabb2{glm::vec3{2.0f, 3.0f, 4.0f}, glm::vec3{5.0f, 6.0f, 7.0f}};
    signal_tracer::AABB aabb3{aabb, aabb2};
    EXPECT_TRUE(aabb3 == (signal_tracer::AABB {glm::vec3{1.0f, 2.0f, 3.0f}, glm::vec3{5.0f, 6.0f, 7.0f}}));
}

TEST(AABBTest, AABBCombinationThreeAABBs) {
    glm::vec3 p1{1.0f, 5.0f, 3.0f};
    glm::vec3 p2{4.0f, 2.0f, 6.0f};
    glm::vec3 p3{3.0f, 6.0f, 4.0f};
    signal_tracer::AABB aabb{p1, p2, p3};
    glm::vec3 p4{2.0f, 3.0f, 4.0f};
    glm::vec3 p5{5.0f, 6.0f, 7.0f};
    glm::vec3 p6{4.0f, 7.0f, 5.0f};
    signal_tracer::AABB aabb2{p4, p5, p6};
    glm::vec3 p7{3.0f, 9.0f, 5.0f};
    glm::vec3 p8{6.0f, 7.0f, 8.0f};
    glm::vec3 p9{5.0f, 8.0f, 6.0f};
    signal_tracer::AABB aabb3{p7, p8, p9};
    signal_tracer::AABB aabb4{aabb, aabb2, aabb3};
    EXPECT_TRUE(aabb4 == (signal_tracer::AABB {glm::vec3{1.0f, 2.0f, 3.0f}, glm::vec3{6.0f, 9.0f, 8.0f}}));
}

TEST(AABBTest, AABBCombinationBoxPoint) {
    glm::vec3 p1{1.0f, 5.0f, 3.0f};
    glm::vec3 p2{4.0f, 2.0f, 6.0f};
    glm::vec3 p3{3.0f, 6.0f, 4.0f};
    signal_tracer::AABB aabb{p1, p2, p3};
    glm::vec3 p4{2.0f, -1.0f, 9.0f};
    signal_tracer::AABB aabb2{aabb, p4};
    EXPECT_TRUE(aabb2 == (signal_tracer::AABB {glm::vec3{1.0f, -1.0f, 3.0f}, glm::vec3{4.0f, 6.0f, 9.0f}}));
}



#endif // !AABB_TEST_HPP