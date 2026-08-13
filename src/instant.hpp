#pragma once

#include <chrono>

#include "exception.hpp"
#include "duration.hpp"

namespace stu {

class Instant {
public:
    Instant() = delete;
    ~Instant() = default;

    static Instant now() {
        return Instant(std::chrono::steady_clock::now());
    }

    Duration operator-(const Instant& other) const {
        auto diff = _time_point - other._time_point;
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
        return Duration::from_ns(ns);
    }

    Instant operator+(const Duration& duration) const {
        auto new_time_point = _time_point + std::chrono::nanoseconds(duration.in_ns());
        return Instant(new_time_point);
    }

    Instant operator-(const Duration& duration) const {
        auto new_time_point = _time_point - std::chrono::nanoseconds(duration.in_ns());
        return Instant(new_time_point);
    }

private:
    explicit Instant(std::chrono::steady_clock::time_point tp) : _time_point(tp) {}

    std::chrono::steady_clock::time_point _time_point;
};

} // namespace stu