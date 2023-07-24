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

    class Timer {
    private:
        // Type aliases to make accessing nested type easier
        using Clock = std::chrono::steady_clock;
        using Second = std::chrono::duration<double, std::ratio<1> >;

        std::chrono::time_point<Clock> m_begin{ Clock::now() };

    public:

        void reset() {
            m_begin = Clock::now();
        }

        double elapsed() const {
            return std::chrono::duration_cast<Second>(Clock::now() - m_begin).count();
        }

        void execution_time() {

            auto sec = elapsed();
            auto h = static_cast<double>(sec) / 3600.0;
            if (h < 1.0) {
                auto m = static_cast<double>(sec) / 60.0;
                if (m < 1.0) {
                    std::clog << "Execution time: " << sec << " seconds" << std::endl << std::endl;
                    return;
                }
                auto s = static_cast<double>(static_cast<int>(sec) % 60);
                std::clog << "Execution time: " << static_cast<int>(m) << " minutes " << s << " seconds" << std::endl << std::endl;
                return;
            }
            auto m = static_cast<double>(static_cast<int>(sec) % 3600) / 60.0;
            auto s = static_cast<double>((static_cast<int>(sec) % 3600) % 60);
            std::clog << "Execution time: " << static_cast<int>(h) << " hours " << static_cast<int>(m) << " minutes " << s << " seconds" << std::endl << std::endl;
        };
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
};


#endif // !UTILS_HPP