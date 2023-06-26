#pragma once

#ifndef INTERVAL_TEST_HPP
#define INTERVAL_TEST_HPP

#include "interval.hpp"
#include <gtest/gtest.h>

/*
    ----------------------------------------
    Interval Tests
    ----------------------------------------
*/
TEST(IntervalTest, IntervalConstructor) {
    signal_tracer::Interval interval1{1.0f, 2.0f};
    EXPECT_EQ(interval1.min(), 1.0f);
    EXPECT_EQ(interval1.max(), 2.0f);
    EXPECT_EQ(interval1.length(), 1.0f);
    EXPECT_EQ(interval1[0], 1.0f);
    EXPECT_EQ(interval1[1], 2.0f);
    EXPECT_TRUE(interval1.contains(1.5f));
    EXPECT_TRUE(interval1.surrounds(1.5f));
    EXPECT_TRUE(interval1 == (signal_tracer::Interval {1.0f, 2.0f}));
}

TEST(IntervalTest, TwoIntervals) {
    signal_tracer::Interval interval1{1.0f, 2.0f};
    signal_tracer::Interval interval2{3.0f, 4.0f};
    EXPECT_TRUE(interval1 == (signal_tracer::Interval {1.0f, 2.0f}));
    EXPECT_TRUE(interval2 == (signal_tracer::Interval {3.0f, 4.0f}));
    EXPECT_TRUE(interval1 != interval2);
}

TEST(IntervalTest, IntervalCopyConstructor) {
    signal_tracer::Interval interval1(1.0f, 2.0f);
    signal_tracer::Interval interval2(interval1);
    EXPECT_TRUE(interval1 == interval2);
}

TEST(IntervalTest, IntervalMoveConstructor) {
    signal_tracer::Interval interval1(1.0f, 2.0f);
    signal_tracer::Interval interval2(std::move(interval1));
    EXPECT_TRUE(interval2 == (signal_tracer::Interval {1.0f, 2.0f}));
}

TEST(IntervalTest, IntervalAssignmentOperator) {
    signal_tracer::Interval interval1{1.0f, 2.0f};
    signal_tracer::Interval interval2{3.0f, 4.0f};
    interval2 = interval1;
    EXPECT_TRUE(interval1 == interval2);
}

TEST(IntervalTest, IntervalMoveAssignmentOperator) {
    signal_tracer::Interval interval1{1.0f, 2.0f};
    signal_tracer::Interval interval2{3.0f, 4.0f};
    interval2 = std::move(interval1);
    EXPECT_TRUE(interval2 == (signal_tracer::Interval {1.0f, 2.0f}));
}

TEST(IntervalTest, CombineIntervals) {
    signal_tracer::Interval interval1{1.0f, 2.0f};
    signal_tracer::Interval interval2{1.5f, 4.0f};
    signal_tracer::Interval interval3 = signal_tracer::Interval::get_union_interval(interval1, interval2);
    EXPECT_TRUE(interval3 == (signal_tracer::Interval {1.0f, 4.0f}));

    signal_tracer::Interval interval4 = signal_tracer::Interval::get_inner_interval(interval1, interval2);
    EXPECT_TRUE(interval4 == (signal_tracer::Interval {1.5f, 2.0f}));
}

TEST(IntervalTest, EmptyInterval) {
    signal_tracer::Interval interval1{1.0f, 2.0f};
    signal_tracer::Interval interval2{2.0f, 1.0f};
    signal_tracer::Interval interval3{1.0f, 1.0f};
    signal_tracer::Interval interval4{2.0f, 2.0f};
    signal_tracer::Interval interval5{1.0f, 2.0f};
    signal_tracer::Interval interval6{1.1f, 1.0f};
    signal_tracer::Interval interval7{};
    EXPECT_FALSE(interval1.is_empty());
    EXPECT_TRUE(interval2.is_empty());
    EXPECT_FALSE(interval3.is_empty());
    EXPECT_FALSE(interval4.is_empty());
    EXPECT_FALSE(interval5.is_empty());
    EXPECT_TRUE(interval6.is_empty());
    EXPECT_TRUE(interval7.is_empty());
}

#endif // !INTERVAL_TEST_HPP