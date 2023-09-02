#include "utils.hpp"

namespace Utils {
    void Timer::reset() {
        m_begin = Clock::now();
    }

    double Timer::elapsed() const {
        return std::chrono::duration_cast<Second>(Clock::now() - m_begin).count();
    }

    void Timer::execution_time() {

        auto sec = elapsed();
        auto h = static_cast<double>(sec) / 3600.0;
        if (h < 1.0) {
            auto m = static_cast<double>(sec) / 60.0;
            if (m < 1.0) {
                std::clog << "Execution time: " << sec << " seconds" << std::endl << std::endl;
                return;
            }
            auto s = static_cast<double>(static_cast<int>(sec) % 60);
            std::clog << "Execution time: " << static_cast<int>(m) << " minutes " << s << " seconds" << std::endl << std::endl;
            return;
        }
        auto m = static_cast<double>(static_cast<int>(sec) % 3600) / 60.0;
        auto s = static_cast<double>((static_cast<int>(sec) % 3600) % 60);
        std::clog << "Execution time: " << static_cast<int>(h) << " hours " << static_cast<int>(m) << " minutes " << s << " seconds" << std::endl << std::endl;
    };
}