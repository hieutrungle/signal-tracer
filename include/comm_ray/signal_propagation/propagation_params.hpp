#pragma once

#ifndef PROPAGATION_PARAMS_HPP
#define PROPAGATION_PARAMS_HPP

#include "glm/glm.hpp"
#include <string>

namespace SignalTracer {
    struct PropagationParams {
        float frequency{ 5.4e9f }; // Hz
        float tx_power{ 10.0f }; // dBm
        float tx_gain{ 1.0f }; // dB
        float rx_gain{ 1.0f }; // dB
        std::string polarization{ "TM" };
    };
}

#endif // !PROPAGATION_PARAMS_HPP