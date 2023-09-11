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
};

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

    class Timer {
    public:

        /// @brief Reset the timer to zero
        void reset();

        /// @brief Return the elapsed time in seconds
        /// @return The elapsed time in seconds
        double elapsed() const;

        /// @brief Print the execution time in seconds, minutes, hours
        void execution_time();

    private:
        // Type aliases to make accessing nested type easier
        using Clock = std::chrono::steady_clock;
        using Second = std::chrono::duration<double, std::ratio<1> >;
        std::chrono::time_point<Clock> m_begin{ Clock::now() };

    };

    // void show_FPS(std::vector<Model>& model_list, double& time, int& skip) {
    //     double prev_time = time;
    //     time = glfwGetTime();

    //     float delta_time = (float) (time - prev_time);

    //     ++skip; // Avoids std::cout slowing down the program too much. 
    //     if (skip == 60) // 60 Is simply so that for VSync 60FPS it displays here once per second (Any number is OK)
    //     {
    //         skip = 0;
    //         for (unsigned int i = 0; i < model_list.size(); ++i) {
    //             if (model_list[i].draw_method == 0)
    //                 std::cout << "   Draw method: Multiple Meshes --- Draw calls: " << model_list[i].num_meshes << " --- Model name : " << model_list[i].model_name << "\n";
    //             if (model_list[i].draw_method == 1)
    //                 std::cout << "   Draw method: 5VBO --- Draw calls: 1 --- Model name : " << model_list[i].model_name << "\n";
    //             if (model_list[i].draw_method == 2)
    //                 std::cout << "   Draw method: 1VBO --- Draw calls: 1 --- Model name : " << model_list[i].model_name << "\n";
    //         }
    //         std::cout << "   FPS: " << ((1.0f / 60) / delta_time) * 60 << "\n\n";
    //     }
    // }

    inline float dB_to_linear(float dB) {
        return std::pow(10.0f, dB / 10.0f);
    }

    inline float linear_to_dB(float linear) {
        return 10.0f * std::log10(linear);
    }

    inline glm::vec3 spherical2cartesian(float yaw, float pitch) {
        glm::vec3 cartesian;
        cartesian.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cartesian.y = sin(glm::radians(pitch));
        cartesian.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return cartesian;
    }

    inline void cartesian2spherical(const glm::vec3& position, const glm::vec3& target, float& yaw, float& pitch) {
        // Convert from the position point and the target point to spherical coordinates
        glm::vec3 cartesian = position - target;
        yaw = -atan2(cartesian.z, cartesian.x) * 180 / Constant::PI;
        pitch = -atan2(cartesian.y, sqrt(cartesian.x * cartesian.x + cartesian.z * cartesian.z)) * 180 / Constant::PI;
    }

    inline std::vector<glm::vec3> get_fibonacci_lattice(int num_points) {
        // https://extremelearning.com.au/evenly-distributing-points-on-a-sphere/

        float golden_ratio = (1.0f + glm::sqrt(5.0f)) / 2.0f;

        int min_n{}, max_n{};
        if (num_points % 2 == 0) {
            min_n = -num_points / 2;
            max_n = num_points / 2 - 1;
        }
        else {
            min_n = -(num_points - 1) / 2;
            max_n = (num_points - 1) / 2;
        }

        std::vector<glm::vec3> points(num_points);
        for (int i = min_n; i <= max_n; ++i) {
            // compute for the right hand coordinate system
            // theta = pitch, phi = yaw
            float theta{ float(acos(1.0f - 2.0f * (i - min_n + 0.5) / num_points) - Constant::PI / 2.0f) };
            float phi{};
            if (num_points % 2 == 0) {
                phi = 2 * Constant::PI * (i + 0.5) / golden_ratio;
            }
            else {
                phi = 2 * Constant::PI * i / golden_ratio;
            }

            phi = glm::degrees(phi);
            theta = glm::degrees(theta);
            // std::cout << "theta: " << theta << " phi: " << phi << std::endl;
            points[i - min_n] = glm::normalize(spherical2cartesian(phi, theta));
        }

        return points;
    }

    inline double degrees_to_radians(double degrees) {
        return degrees * Constant::PI / 180.0;
    }

    template<typename T>
    inline size_t vector_sizeof(const typename std::vector<T>& vec) {
        return sizeof(T) * vec.size();
    }
};


#endif // !UTILS_HPP