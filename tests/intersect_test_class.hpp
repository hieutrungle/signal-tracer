#pragma once

#ifndef INTERSECTION_TEST_CLASS_HPP
#define INTERSECTION_TEST_CLASS_HPP

#include "aabb.hpp"
#include "bvh_map.hpp"
#include "interval.hpp"
#include "intersect_record.hpp"
#include "hittable_list.hpp"
#include "utils.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include "ray.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

/*
    ----------------------------------------
    Triangle Tests
    ----------------------------------------
*/

class IntersectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        using signal_tracer::Triangle;
        using signal_tracer::Material;
        p1 = glm::vec3{ 0.0f, 0.0f, 0.0f };
        p2 = glm::vec3{ 1.0f, 0.0f, 0.0f };
        p3 = glm::vec3{ 0.0f, 1.0f, 0.0f };
        p4 = glm::vec3{ -1.0f, 4.0f, -7.0f };
        p_material = std::make_shared<Material>();
        triangle1 = std::make_shared<Triangle>(p1, p2, p3, p_material);
        triangle2 = std::make_shared<Triangle>(p4, p2, p3, p_material);
        std::vector<std::shared_ptr<Triangle>> trangles {{triangle1, triangle2}};
        model1 = signal_tracer::HittableList(trangles);
        bvh = std::make_shared<signal_tracer::BVH>(model1);
    }

    glm::vec3 p1{};
    glm::vec3 p2{};
    glm::vec3 p3{};
    glm::vec3 p4{};
    std::shared_ptr<signal_tracer::Material> p_material{};
    std::shared_ptr<signal_tracer::Triangle> triangle1{};
    std::shared_ptr<signal_tracer::Triangle> triangle2{};
    signal_tracer::HittableList model1{};
    signal_tracer::IntersectRecord record{};
    signal_tracer::Interval interval{0.0f, 100.0f};
    std::shared_ptr<signal_tracer::BVH> bvh{};

    signal_tracer::Ray ray1{ glm::vec3{0.0f, 0.0f, 2.0f}, glm::vec3{0.0f, 0.0f, -1.0f} };
    signal_tracer::Ray ray2{ glm::vec3{2.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f} };
    signal_tracer::Ray ray3{ glm::vec3{0.0f, 2.0f, 2.0f}, glm::vec3{0.0f, 0.0f, 1.0f} };

};


#endif // !INTERSECTION_TEST_CLASS_HPP