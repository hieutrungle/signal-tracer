#include "receiver.hpp"

namespace SignalTracer {

    Receiver::Receiver() : RadioDevice{} {};

    Receiver::Receiver(const int& id, const glm::vec3& position, const float& gain, const float& pitch, const float& yaw, float radius)
        : RadioDevice{ position, pitch, yaw, radius }
        , m_id{ id }
        , m_gain{ gain } {}

    std::ostream& Receiver::print(std::ostream& out) const {
        RadioDevice::print(out);
        return out;
    }
}