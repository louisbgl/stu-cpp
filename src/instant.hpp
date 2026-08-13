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
        int64_t tp_ns = _time_point.time_since_epoch().count();
        int64_t dur_ns = duration.in_ns();
        if ((dur_ns > 0 && tp_ns > INT64_MAX - dur_ns))
            throw StuException("Instant addition overflow: " + std::to_string(tp_ns) + " + " + std::to_string(dur_ns));
        if ((dur_ns < 0 && tp_ns < INT64_MIN - dur_ns))
            throw StuException("Instant addition underflow: " + std::to_string(tp_ns) + " + " + std::to_string(dur_ns));
        
        auto new_time_point = _time_point + std::chrono::nanoseconds(dur_ns);
        return Instant(new_time_point);
    }

    Instant operator-(const Duration& duration) const {
        int64_t tp_ns = _time_point.time_since_epoch().count();
        int64_t dur_ns = duration.in_ns();
        if ((dur_ns > 0 && tp_ns < INT64_MIN + dur_ns))
            throw StuException("Instant subtraction underflow: " + std::to_string(tp_ns) + " - " + std::to_string(dur_ns));
        if ((dur_ns < 0 && tp_ns > INT64_MAX + dur_ns))
            throw StuException("Instant subtraction overflow: " + std::to_string(tp_ns) + " - " + std::to_string(dur_ns));
        
        auto new_time_point = _time_point - std::chrono::nanoseconds(dur_ns);
        return Instant(new_time_point);
    }

    bool operator==(const Instant& other) const {
        return _time_point == other._time_point;
    }

    bool operator!=(const Instant& other) const {
        return _time_point != other._time_point;
    }

    bool operator<(const Instant& other) const {
        return _time_point < other._time_point;
    }

    bool operator<=(const Instant& other) const {
        return _time_point <= other._time_point;
    }

    bool operator>(const Instant& other) const {
        return _time_point > other._time_point;
    }

    bool operator>=(const Instant& other) const {
        return _time_point >= other._time_point;
    }

private:
    explicit Instant(std::chrono::steady_clock::time_point tp) : _time_point(tp) {}

    std::chrono::steady_clock::time_point _time_point;
};

} // namespace stu