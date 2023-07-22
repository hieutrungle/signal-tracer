#include "transmitter.hpp"

namespace SignalTracer {

    Transmitter::Transmitter() : RadioStation{} {};

    Transmitter::Transmitter(const int& id, const glm::vec3& position, const float& frequency, const float& power, const float& gain, const float& pitch, const float& yaw, float radius)
        : RadioStation{ position, pitch, yaw, radius }
        , m_id{ id }
        , m_frequency{ frequency }
        , m_power{ power }
        , m_gain{ gain } {}

    std::ostream& Transmitter::print(std::ostream& out) const {
        RadioStation::print(out);
        return out;
    }
}