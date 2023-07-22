#include "radio_station.hpp"

namespace SignalTracer {

    RadioStation::RadioStation() : Cubesphere{} {};

    RadioStation::RadioStation(const glm::vec3& position, const float& pitch, const float& yaw, float radius)
        : Cubesphere{ radius }
        , m_position{ position }
        , m_pitch{ pitch }
        , m_yaw{ yaw } {
        set_model_mat(glm::translate(get_model_mat(), m_position));
    }

    std::ostream& RadioStation::print(std::ostream& out) const {
        out << "Transmitter: " << "\n";
        out << "Position: " << glm::to_string(m_position) << "\n";
        out << "Orientation: " << "Pitch: " << m_pitch << " Yaw: " << m_yaw << "\n";
        out << "Radius: " << get_radius() << "\n";
        return out;
    }

    void RadioStation::set_position(const glm::vec3& position) {
        m_position = position;
        set_model_mat(glm::translate(glm::mat4(1.0f), m_position));
    }

    void RadioStation::set_pitch(const float& pitch) {
        m_pitch = pitch;
        set_model_mat(glm::rotate(get_model_mat(), glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    void RadioStation::set_yaw(const float& yaw) {
        m_yaw = yaw;
        set_model_mat(glm::rotate(get_model_mat(), glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    void RadioStation::update_location(const glm::vec3& position, const float& pitch, const float& yaw) {
        set_position(position);
        set_pitch(pitch);
        set_yaw(yaw);
    }

    void RadioStation::update_orientation(const float& pitch, const float& yaw) {
        set_pitch(pitch);
        set_yaw(yaw);
    }

}