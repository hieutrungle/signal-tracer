#include "aabb_test.hpp"
#include "intersect_hittablelist_test.hpp"
#include "intersection_test.hpp"
#include "interval_test.hpp"
#include "ray_test.hpp"
#include "triangle_test.hpp"
#include <iostream>
#include <gtest/gtest.h>
#include <vector>


int main(int argc, char** argv) {
    std::cout << "Running main() from gtest_main.cpp\n";

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}