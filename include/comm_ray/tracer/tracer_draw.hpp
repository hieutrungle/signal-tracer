#pragma once

#ifndef TRACER_DRAW_HPP
#define TRACER_DRAW_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cubesphere.h"
#include "drawable.hpp"
#include "line.hpp"
#include <vector>

namespace signal_tracer {
    class TracerDrawing {
    public:
        TracerDrawing() = default;
        TracerDrawing(const TracerDrawing& other) = default;
        TracerDrawing(TracerDrawing&& other) noexcept = default;
        TracerDrawing& operator=(const TracerDrawing& other) = default;
        TracerDrawing& operator=(TracerDrawing&& other) noexcept = default;
        ~TracerDrawing() = default;



    protected:
        // Remember to activate the program before setting uniforms!


    private:
        Cubesphere m_radio_object{ 0.25f, 3, true };
        std::vector<signal_tracer::Line> m_lines{};
        int m_display_reflection_count{ 0 };
    };
}


#endif // !TRACER_DRAW_HPP