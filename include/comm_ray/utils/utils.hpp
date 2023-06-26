#pragma once

#ifndef UTILS_HPP
#define UTILS_HPP

#include "constant.hpp"
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>

// Unused macro
#define UTILS_UNUSED __attribute__((unused))
#define UTILS_UNUSED_PARAM(name) unused_ ## name UTILS_UNUSED

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Utility Functions
// This header-only Random namespace implements a self-seeding Mersenne Twister
// It can be included into as many code files as needed (The inline keyword avoids ODR violations)
namespace Random {
    inline std::mt19937 init() {
        std::random_device rd;

        // Create seed_seq with high-res clock and 7 random numbers from std::random_device
        std::seed_seq ss{
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
                rd(), rd(), rd(), rd(), rd(), rd(), rd() };

        return std::mt19937{ ss };
    }

    // Here's our std::mt19937 PRNG object
    // The inline keyword also means we only have one global instance for our whole program
    inline std::mt19937 mt{ init() };

    // Generate a random number between [min, max] (inclusive)
    inline int get_int(int min, int max) {
        std::uniform_int_distribution dist{ min, max };
        return dist(mt); // and then generate a random number from our global generator
    }

    inline double get_double(double min, double max) {
        std::uniform_real_distribution dist{ min, max };
        return dist(mt);
    }
};

inline double degrees_to_radians(double degrees) {
    return degrees * Constant::PI / 180.0;
}

inline double random_double() {
    // Returns a random real in [0, 1)
    // static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    // return distribution(Random::mt);
    return Random::get_double(0.0, 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min, max)
    // static std::uniform_real_distribution<double> distribution(min, max);
    // return distribution(Random::mt);
    return Random::get_double(min, max);
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min, max]
    // static std::uniform_int_distribution<int> distribution(min, max);
    // return distribution(Random::mt);
    return Random::get_int(min, max);
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;

    return x;
}

namespace Utils {
    template<typename T>
    inline std::string round_to_string(T value, int precision) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }
};


#endif // !UTILS_HPP