#pragma once

#ifndef THEORATICAL_MODEL_HPP
#define THEORATICAL_MODEL_HPP

namespace SignalTracer {
    class TheoraticalModel {
    public:
        TheoraticalModel() = default;

        virtual ~TheoraticalModel() = default;

        virtual float calculate_path_loss(float distance) const = 0;
    };
}



#endif //! THEORATICAL_MODEL_HPP