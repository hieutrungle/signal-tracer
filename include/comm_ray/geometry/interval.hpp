#pragma once

#ifndef INTERVAL_H
#define INTERVAL_H

#include "utils.hpp"
#include "constant.hpp"

namespace SignalTracer {

    /*
        Interval class
        Can be used to represent a range of values
        Constructor:
            Interval() : m_min{ Constant::INFINITY_POS }, m_max{ Constant::INFINITY_NEG } {}
            Interval(T min, T max) : m_min(min), m_max(max) {}
    */
    template<typename T>
    class IntervalT {
    public:
        // default to empty interval
        IntervalT() : m_min{ static_cast<T>(Constant::INFINITY_POS) }, m_max{ static_cast<T>(Constant::INFINITY_NEG) } {}

        // Initialize an interval with two values
        IntervalT(T min, T max) : m_min(min), m_max(max) {}

        // copy constructor
        IntervalT(const IntervalT<T>& interval)
            : m_min{ interval.m_min }, m_max{ interval.m_max } {}

        // move constructor
        IntervalT(IntervalT<T>&& interval) noexcept
            : m_min{ interval.m_min }, m_max{ interval.m_max } {}

        // assignment operator
        IntervalT<T>& operator=(const IntervalT<T>& interval) {
            m_min = interval.m_min;
            m_max = interval.m_max;
            return *this;
        }

        // move assignment operator
        IntervalT<T>& operator=(IntervalT<T>&& interval) noexcept {
            m_min = interval.m_min;
            m_max = interval.m_max;
            return *this;
        }

        bool operator==(const IntervalT<T>& interval) const {
            return m_min == interval.m_min && m_max == interval.m_max;
        }

        bool operator!=(const IntervalT<T>& interval) const {
            return m_min != interval.m_min || m_max != interval.m_max;
        }

        // array subscript operator
        T operator[](int i) const {
            if (i == 0) return m_min;
            return m_max;
        }

        // cout
        friend std::ostream& operator<<(std::ostream& os, const IntervalT<T>& interval) {
            os << "[" << interval.m_min << ", " << interval.m_max << "]";
            return os;
        }

        bool contains(T x) const {
            return m_min <= x && x <= m_max;
        }

        bool surrounds(T x) const {
            return m_min < x && x < m_max;
        }

        IntervalT expand(T delta) const {
            const auto padding = delta / 2;
            return IntervalT(m_min - padding, m_max + padding);
        }

        // Get min value
        T min() const { return m_min; };
        // Get max value
        T max() const { return m_max; };

        // Set min value
        void min(T min) { m_min = min; };
        // Set max value
        void max(T max) { m_max = max; };

        bool is_empty() const {
            return m_min > m_max;
        }

        const T length() const {
            return m_max - m_min;
        }

        static IntervalT<T> empty() {
            return IntervalT<T>((Constant::INFINITY_POS_T<T>), (Constant::INFINITY_NEG_T<T>));
        }

        static IntervalT<T> universe() {
            return IntervalT<T>((Constant::INFINITY_NEG_T<T>), (Constant::INFINITY_POS_T<T>));
        }

        static IntervalT<T> get_union_interval(const IntervalT<T>& interval1, const IntervalT<T>& interval2) {
            return IntervalT<T>(fmin(interval1.min(), interval2.min()), fmax(interval1.max(), interval2.max()));
        }

        static IntervalT<T> get_inner_interval(const IntervalT<T>& interval1, const IntervalT<T>& interval2) {
            return IntervalT<T>(fmax(interval1.min(), interval2.min()), fmin(interval1.max(), interval2.max()));
        }

    private:
        T m_min{}, m_max{};
    };

    using Interval = IntervalT<float>;
}


#endif