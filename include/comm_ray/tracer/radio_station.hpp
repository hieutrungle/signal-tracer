#pragma once

#ifndef RADIO_STATION_HPP
#define RADIO_STATION_HPP

#include "glm/glm.hpp"
#include "utils.hpp"

namespace signal_tracer {

    enum class StationColor {
        red,
        green,
        blue,
        yellow,
        cyan,
        magenta,
        orange,
        purple,
        pink,
        brown,
    };

    constexpr auto operator+(StationColor color) noexcept {
        return static_cast<std::underlying_type_t<StationColor>>(color);
    }

    const glm::vec3 get_station_color(StationColor color) {
        switch (color) {
        case StationColor::red:
            return Constant::RED;
        case StationColor::green:
            return Constant::GREEN;
        case StationColor::blue:
            return Constant::BLUE;
        case StationColor::yellow:
            return Constant::YELLOW;
        case StationColor::cyan:
            return Constant::CYAN;
        case StationColor::magenta:
            return Constant::MAGENTA;
        case StationColor::orange:
            return Constant::ORANGE;
        case StationColor::purple:
            return Constant::PURPLE;
        case StationColor::pink:
            return Constant::PINK;
        case StationColor::brown:
            return Constant::BROWN;
        default:
            return Constant::DARK_GRAY;
        }
    }

}

#endif // !RADIO_STATION_HPP