#pragma once

#ifndef THEORATICAL_MODEL_HPP
#define THEORATICAL_MODEL_HPP

#include "glm/glm.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace SignalTracer {
    class TheoraticalModel {
    public:
        TheoraticalModel() = default;
        virtual ~TheoraticalModel() = default;

        /// @brief Calculate the free space loss in dB using the Friss formula
        /// @param distance Meters
        /// @param frequency Frequency in Hz
        /// @return Free space loss in dB
        float calc_free_space_path_loss(float distance, float frequency) const {
            // constant 10*log((4*pi/c)^2) = -147.55221677811664
            return 20.0f * std::log10(distance) + 20.0f * std::log10(frequency) - 147.55221677811664f;
        }

        /// @brief Calculate the received power in dBm using the Friss formula
        /// @param tx_power Power of transmitter in dBm
        /// @param tx_gain Gain of transmitter in dB
        /// @param rx_gain Gain of receiver in dB
        /// @param distance Distance between transmitter and receiver in meters
        /// @param frequency Frequency in Hz
        /// @return Received power in dBm
        float calc_received_power_freespace(float tx_power, float tx_gain, float rx_gain, float distance, float frequency) const {
            float free_space_loss = calc_free_space_path_loss(distance, frequency);
            return tx_power + tx_gain + rx_gain - free_space_loss;
        }

        float calc_reflection_coefficient(glm::vec3 start, glm::vec3 mid, glm::vec3 end, float outter_triangle_permittivity, float inner_triangle_permittivity, std::string polar) const {
            return 0.0f;
        }

    };
}



#endif //! THEORATICAL_MODEL_HPP