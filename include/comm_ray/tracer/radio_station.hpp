#pragma once

#ifndef RADIO_STATION_HPP
#define RADIO_STATION_HPP

#include "glm/glm.hpp"
#include "utils.hpp"

namespace signal_tracer {

    // Station color
    enum class LineColor {
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

    constexpr auto operator+(LineColor color) noexcept {
        return static_cast<std::underlying_type_t<LineColor>>(color);
    }

    const glm::vec3 get_station_color(LineColor color) {
        switch (color) {
        case LineColor::red:
            return Constant::RED;
        case LineColor::green:
            return Constant::GREEN;
        case LineColor::blue:
            return Constant::BLUE;
        case LineColor::yellow:
            return Constant::YELLOW;
        case LineColor::cyan:
            return Constant::CYAN;
        case LineColor::magenta:
            return Constant::MAGENTA;
        case LineColor::orange:
            return Constant::ORANGE;
        case LineColor::purple:
            return Constant::PURPLE;
        case LineColor::pink:
            return Constant::PINK;
        case LineColor::brown:
            return Constant::BROWN;
        default:
            return Constant::GRAY;
        }
    }

    // Light reflections
    enum class StationColor {
        light_red,
        light_green,
        light_blue,
        light_yellow,
        light_cyan,
        light_magenta,
        light_orange,
        light_purple,
        light_pink,
        light_brown,
    };

    constexpr auto operator+(StationColor color) noexcept {
        return static_cast<std::underlying_type_t<StationColor>>(color);
    }

    const glm::vec3 get_station_color(StationColor color) {
        switch (color) {
        case StationColor::light_red:
            return Constant::LIGHT_RED;
        case StationColor::light_green:
            return Constant::LIGHT_GREEN;
        case StationColor::light_blue:
            return Constant::LIGHT_BLUE;
        case StationColor::light_yellow:
            return Constant::LIGHT_YELLOW;
        case StationColor::light_cyan:
            return Constant::LIGHT_CYAN;
        case StationColor::light_magenta:
            return Constant::LIGHT_MAGENTA;
        case StationColor::light_orange:
            return Constant::LIGHT_ORANGE;
        case StationColor::light_purple:
            return Constant::LIGHT_PURPLE;
        case StationColor::light_pink:
            return Constant::LIGHT_PINK;
        case StationColor::light_brown:
            return Constant::LIGHT_BROWN;
        default:
            return Constant::LIGHT_GRAY;
        }
    }
}

#endif // !RADIO_STATION_HPP