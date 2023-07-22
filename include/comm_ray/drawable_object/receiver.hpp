#pragma once

#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "radio_station.hpp"
#include "reflection_record.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <unordered_map>


namespace SignalTracer {
    class Receiver : public RadioStation {
    public:
        Receiver();
        Receiver(const int& id, const glm::vec3& position, const float& gain, const float& pitch = 0.0, const float& yaw = 0.0f, float radius = 0.25f);

        std::ostream& print(std::ostream& out) const override;

        int get_id() const { return m_id; }
        float get_gain() const { return m_gain; }
        const std::unordered_map<int, std::vector<ReflectionRecord>>& get_reflection_records() { return m_reflection_records; }

        void set_gain(const float& gain) { m_gain = gain; }

        void add_reflection_record(const int& transmitter_id, const ReflectionRecord& record) { m_reflection_records[transmitter_id].push_back(record); }
        void add_reflection_records(const int& transmitter_id, const std::vector<ReflectionRecord>& records) { m_reflection_records[transmitter_id] = records; }

        void clear_reflection_records() { m_reflection_records.clear(); }
        void clear_reflection_records(const int& transmitter_id) { m_reflection_records[transmitter_id].clear(); }

    private:
        int m_id{ 0 };
        float m_gain{ 1.0f };
        std::unordered_map<int, std::vector<ReflectionRecord>> m_reflection_records{};
    };
}

#endif // !RECEIVER_HPP