#pragma once

#ifndef THEORATICAL_MODEL_HPP
#define THEORATICAL_MODEL_HPP

#include "constant.hpp"
#include "path_record.hpp"
#include "propagation_params.hpp"
#include "material.hpp"
#include "glm/glm.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace SignalTracer {
    class PropagationModel {
    public:

        /// @brief Construct a new Propagation Model object
        /// @param frequency Hz
        /// @param tx_power Power of transmitter in dBm
        /// @param tx_gain Gain of transmitter in dB
        /// @param rx_gain Gain of receiver in dB
        /// @param polarization Polization of the sending and receiving antenna, either "TM" or "TE"
        PropagationModel(const float& frequency, const float& tx_power, const float& tx_gain, const float& rx_gain, const std::string& polarization = "TM", const float& tx_permittivity = 1.0f, const float& tranmission_speed = Constant::LIGHT_SPEED / std::sqrt(1.0f))
            :m_frequency{ frequency }
            , m_tx_power{ tx_power }
            , m_tx_gain{ tx_gain }
            , m_rx_gain{ rx_gain }
            , m_polarization{ polarization }
            , m_tx_permittivity{ tx_permittivity }
            , m_tranmission_speed{ tranmission_speed } {
            // error if polarization is not "TM" or "TE"
            if (polarization != "TM" && polarization != "TE") {
                std::cerr << "Invalid polarization" << std::endl;
            }
        }

        /// @brief Construct a new Propagation Model object
        /// @param prop_params Propagation parameters are stored in a struct
        PropagationModel(const PropagationParams& prop_params)
            : m_frequency{ prop_params.frequency }, m_tx_power{ prop_params.tx_power }, m_tx_gain{ prop_params.tx_gain }, m_rx_gain{ prop_params.rx_gain }, m_polarization{ prop_params.polarization } {
            // error if polarization is not "TM" or "TE"
            if (prop_params.polarization != "TM" && prop_params.polarization != "TE") {
                std::cerr << "Invalid polarization" << std::endl;
            }
        }
        virtual ~PropagationModel() = default;

        /// @brief Calculate the free space loss in dB using the Friss formula
        /// @param distance Meters
        /// @param frequency Frequency in Hz
        /// @return Free space loss in dB
        static float calc_free_space_path_loss(float distance, float frequency) {
            // constant 10*log((4*pi/light_speed)^2) = -147.55221677811664
            return 20.0f * std::log10(distance * frequency) - 147.55221677811664f;
        }

        /// @brief Calculate the received power in dBm using the Friss formula
        /// @param tx_power Power of transmitter in dBm
        /// @param tx_gain Gain of transmitter in dB
        /// @param rx_gain Gain of receiver in dB
        /// @param distance Distance between transmitter and receiver in meters
        /// @param frequency Frequency in Hz
        /// @return Received power in dBm
        static float calc_received_power_freespace(float tx_power, float tx_gain, float rx_gain, float distance, float frequency) {
            float free_space_loss = calc_free_space_path_loss(distance, frequency);
            return tx_power + tx_gain + rx_gain - free_space_loss;
        }

        /// @brief Calculate the reflection coefficient of a ray
        /// @param starting_point starting point of the ray
        /// @param anchor_point achor point of the ray
        /// @param ending_point receiving point of the ray
        /// @param eta1 relative permittivity of the outter triangle medium (medium where the ray is coming from)
        /// @param eta2 relative permittivity of the inner triangle medium (medium withing the triangle)
        /// @param polar polization of the sending and receiving antenna
        /// @return coefficient of reflection
        static float calc_reflection_coefficient(const glm::vec3& starting_point, const glm::vec3& anchor_point, const glm::vec3& ending_point, const float& eta1, const float& eta2, const std::string& polar = "TM") {

            float coefficent{};
            float cos_2theta1 = glm::dot(glm::normalize(starting_point - anchor_point), glm::normalize(ending_point - anchor_point));
            float theta1 = std::acos(cos_2theta1) / 2;
            if (std::sqrt(eta1 / eta2) * std::sin(theta1) >= 1.0f) {
                std::cout << "Total internal reflection" << std::endl;
                coefficent = 1.0f;
            }
            else {
                float sqrt_eta1 = std::sqrt(eta1);
                float sqrt_eta2 = std::sqrt(eta2);
                float cos_theta1 = std::cos(theta1);
                float cos_theta2 = std::sqrt(1 - (eta1 / eta2) * std::pow(std::sin(theta1), 2));
                if (polar == "TM") {
                    coefficent = (eta2 * std::cos(theta1) - std::sqrt(eta2 - std::pow(std::sin(theta1), 2))) / (eta2 * std::cos(theta1) + std::sqrt(eta2 - std::pow(std::sin(theta1), 2)));
                }
                else if (polar == "TE") {
                    coefficent = (sqrt_eta1 * cos_theta1 - sqrt_eta2 * cos_theta2) / (sqrt_eta1 * cos_theta1 + sqrt_eta2 * cos_theta2);
                }
                else {
                    std::cerr << "Invalid polarization" << std::endl;
                    coefficent = 0.0f;
                }
            }
            return coefficent;
        }

        static float calc_reflection_coefficient(const float& incident_angle, const float& eta1, const float& eta2, const std::string& polar = "TM") {
            float coefficient{};
            if (std::sqrt(eta1 / eta2) * std::sin(incident_angle) >= 1.0f) {
                std::cout << "Total internal reflection" << std::endl;
                coefficient = 1.0f;
            }
            else {
                float sqrt_eta1 = std::sqrt(eta1);
                float sqrt_eta2 = std::sqrt(eta2);
                float cos_theta1 = std::cos(incident_angle);
                float cos_theta2 = std::sqrt(1 - (eta1 / eta2) * std::pow(std::sin(incident_angle), 2));
                if (polar == "TM") {
                    coefficient = (sqrt_eta2 * cos_theta1 - sqrt_eta1 * cos_theta2) / (sqrt_eta2 * cos_theta1 + sqrt_eta1 * cos_theta2);
                }
                else if (polar == "TE") {
                    coefficient = (sqrt_eta1 * cos_theta1 - sqrt_eta2 * cos_theta2) / (sqrt_eta1 * cos_theta1 + sqrt_eta2 * cos_theta2);
                }
                else {
                    std::cerr << "Invalid polarization" << std::endl;
                    coefficient = 0.0f;
                }

            }
            return std::fabs(coefficient);
        }

        static float calc_reflection_loss(
            const float& frequency,
            const std::vector<glm::vec3>& ray_path,
            const std::vector<float>& material_permittivities,
            const float& tx_permittivity = 1,
            const std::string& polar = "TM") {
            float reflection_coef{ 1.0f };
            float distance{ glm::length(ray_path[0] - ray_path[1]) };
            for (std::size_t i = 0; i < ray_path.size() - 2; i++) {
                float cos_2theta1 = glm::dot(glm::normalize(ray_path[i] - ray_path[i + 1]), glm::normalize(ray_path[i + 2] - ray_path[i + 1]));
                float incident_angle = std::acos(cos_2theta1) / 2;

                reflection_coef *= calc_reflection_coefficient(incident_angle, tx_permittivity, material_permittivities[i], polar);
                distance += glm::length(ray_path[i + 1] - ray_path[i + 2]);
            }
            return 20.0f * std::log10(distance * frequency / reflection_coef) - 147.55221677811664f;
        }

        static float calc_signal_strength(const float& frequency, const std::vector<glm::vec3>& ray_path, const std::vector<float>& material_permittivities, const float& tx_permittivity, const float& tx_power, const float& tx_gain, const float& rx_gain, const std::string& polar = "TM") {
            float reflection_loss = calc_reflection_loss(frequency, ray_path, material_permittivities, tx_permittivity, polar);
            float received_power = tx_power + tx_gain + rx_gain - reflection_loss;
            return received_power;
        }

        static float calc_transmitting_distance(const std::vector<glm::vec3>& ray_path) {
            float distance{ 0.0f };
            for (std::size_t i = 0; i < ray_path.size() - 1; i++) {
                distance += glm::length(ray_path[i] - ray_path[i + 1]);
            }
            return distance;
        }

        static float calc_signal_delay(const float& distance, const float& tranmission_speed = Constant::LIGHT_SPEED) {
            return distance / tranmission_speed;
        }

        static float calc_signal_delay(const std::vector<glm::vec3>& ray_path, const float& tranmission_speed = Constant::LIGHT_SPEED) {
            float distance{ calc_transmitting_distance(ray_path) };
            return distance / tranmission_speed;
        }

        static float calc_signal_phase(const float& time_delay, const float& frequency) {
            return 2 * M_PI * time_delay * frequency;

        }

        static float calc_signal_phase(const std::vector<glm::vec3>& ray_path, const float& frequency, const float& tranmission_speed = Constant::LIGHT_SPEED) {
            float time_delay{ calc_signal_delay(ray_path, tranmission_speed) };
            return 2 * M_PI * time_delay * frequency;
        }

        void calc_all_propagation_properties(std::vector<PathRecord>& ref_records) {
            for (auto& ref_record : ref_records) {

                std::vector<float> material_permittivities{ setup_permittivity(m_frequency, ref_record.get_mat_ptrs()) };

                ref_record.set_signal_loss(calc_reflection_loss(m_frequency, ref_record.get_points(), material_permittivities, m_tx_permittivity, m_polarization));
                ref_record.set_signal_strength(m_tx_power + m_tx_gain + m_rx_gain - ref_record.get_signal_loss());
                ref_record.set_distance(calc_transmitting_distance(ref_record.get_points()));

                // tranmission speed is affected by the medium's permittivity, air_permittivity = 1
                ref_record.set_signal_delay(calc_signal_delay(ref_record.get_distance(), m_tranmission_speed));
                ref_record.set_signal_phase(calc_signal_phase(ref_record.get_signal_delay(), m_frequency));
            }
        }

        void calc_propagation_properties(const std::vector<glm::vec3>& ray_path, const std::vector<float>& material_permittivities, float& signal_strength, float& signal_delay, float& signal_loss) {

            signal_loss = calc_reflection_loss(m_frequency, ray_path, material_permittivities, m_tx_permittivity, m_polarization);
            signal_strength = m_tx_power + m_tx_gain + m_rx_gain - signal_loss;
            signal_delay = calc_signal_delay(ray_path);

        }

        float calc_total_receiving_power(const std::vector<PathRecord>& ref_records) const {
            float total_power{ 0.0f };
            // using accumulate
            total_power = std::accumulate(ref_records.begin(), ref_records.end(), 0.0f, [](float sum, const PathRecord& record) {
                return sum + Utils::dB_to_linear(record.get_signal_strength());
                });
            return Utils::linear_to_dB(total_power);
        }

        friend std::ostream& operator<<(std::ostream& os, const PropagationModel& prop_model) {
            os << "PropagationModel: " << std::endl;
            os << "Frequency: " << prop_model.m_frequency << " Hz" << std::endl;
            os << "Transmitter power: " << prop_model.m_tx_power << " dBm" << std::endl;
            os << "Transmitter gain: " << prop_model.m_tx_gain << " dB" << std::endl;
            os << "Receiver gain: " << prop_model.m_rx_gain << " dB" << std::endl;
            os << "Polarization: " << prop_model.m_polarization << std::endl;
            return os;
        }

    private:
        std::vector<float> setup_permittivity(const float& frequency, const std::vector<std::shared_ptr<Material>>& ref_material_ptrs) {
            std::vector<float> relative_permittivity{};
            for (const auto& material_ptr : ref_material_ptrs) {
                relative_permittivity.emplace_back(material_ptr->calc_real_relative_permittivity(frequency));
            }
            return relative_permittivity;
        }

    private:
        float m_frequency{ 5e9 }; // Hz
        float m_tx_power{ 10 }; // dBm
        float m_tx_gain{ 0 }; // dB
        float m_rx_gain{ 0 }; // dB
        std::string m_polarization{ "TM" };
        float m_tx_permittivity{ Air().calc_real_relative_permittivity(m_frequency) };
        float m_tranmission_speed{ Constant::LIGHT_SPEED / std::sqrt(m_tx_permittivity) };
    };
}



#endif //! THEORATICAL_MODEL_HPP