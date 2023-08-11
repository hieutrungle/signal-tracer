#pragma once

#ifndef _COMM_RAY_TRACER_TRANSMITTER_HPP_
#define _COMM_RAY_TRACER_TRANSMITTER_HPP_

#include "radio_device.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>


namespace SignalTracer {
    class Transmitter : public RadioStation {
    public:
        Transmitter();


        /// @brief Construct a new Transmitter object
        /// @param id The id of the transmitter
        /// @param position The position of the transmitter, in world coordinates
        /// @param frequency The frequency of the transmitter, Hz
        /// @param power The power of the transmitter, dBm
        /// @param gain The gain of the transmitter, dB
        /// @param pitch The pitch of the transmitter, in degrees
        /// @param yaw The yaw of the transmitter, in degrees
        /// @param radius  The radius of the transmitter, in world coordinates
        Transmitter(const int& id, const glm::vec3& position, const float& frequency, const float& power = 10.0f, const float& gain = 1.0f, const float& pitch = 0.0, const float& yaw = 0.0f, float radius = 0.2f);

        std::ostream& print(std::ostream& out) const override;

        int get_id() const { return m_id; }
        float get_power() const { return m_power; }
        float get_frequency() const { return m_frequency; }

        void set_power(const float& power) { m_power = power; }
        void set_frequency(const float& frequency) { m_frequency = frequency; }

    private:
        int m_id{ 0 };
        float m_frequency{ 5.4e9f };
        float m_power{ 10.0f };
        float m_gain{ 1.0f };
    };
}

#endif // !_COMM_RAY_TRACER_TRANSMITTER_HPP_