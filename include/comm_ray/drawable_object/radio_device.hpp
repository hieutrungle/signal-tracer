#pragma once

#ifndef RADIO_STATION_HPP
#define RADIO_STATION_HPP

#include "cubesphere.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>


namespace SignalTracer {
    class RadioDevice : public Cubesphere {
    public:
        RadioDevice();
        RadioDevice(const glm::vec3& position, const float& pitch, const float& yaw, float radius = 0.25f);

        std::ostream& print(std::ostream& out) const override;

        const glm::vec3& get_position() const { return m_position; }
        const float& get_pitch() const { return m_pitch; }
        const float& get_yaw() const { return m_yaw; }

        void set_position(const glm::vec3& position);
        void set_pitch(const float& pitch);
        void set_yaw(const float& yaw);

        void update_location(const glm::vec3& position, const float& pitch, const float& yaw);
        void update_orientation(const float& pitch, const float& yaw);

    private:
        glm::vec3 m_position{ 0.0f };
        float m_pitch{ 0.0f };
        float m_yaw{ 0.0f };
    };
}

#endif // !RADIO_STATION_HPP