#pragma once

#ifndef THEORITICAL_MODEL_HPP
#define THEORITICAL_MODEL_HPP

namespace SignalTracer {
    class TheoriticalModel {
    public:
        TheoriticalModel() = default;

        virtual ~TheoriticalModel() = default;

        virtual float calculate_path_loss(float distance) const = 0;
    };
}



#endif //! THEORITICAL_MODEL_HPP