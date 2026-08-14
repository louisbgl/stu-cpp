#pragma once

#include <chrono>
#include <ostream>

#include "exception.hpp"
#include "duration.hpp"

namespace stu {

/**
 * @brief Represents a specific point in time with nanosecond precision.
 *
 * Instant is a wrapper around std::chrono::steady_clock::time_point.
 * It provides monotonic guarantees, meaning it is safe to use for measuring elapsed time.
 * System time is not monotonic, as internal clock drift happens and is adjusted via NTP.
 * So, Instant can and should be used to measure elapsed time.
 * An Instant can not normally be converted back to a system time, because it is not
 * and should not be calendar aware. It is only for measuring elapsed time.
 */
class Instant {
public:
    Instant() = delete;
    ~Instant() = default;

    // Gets the current time as an Instant
    static Instant now() {
        return Instant(std::chrono::steady_clock::now());
    }

    std::string to_string() const {
        Duration elapsed = Instant::now() - *this;

        if (elapsed.in_ns() < 0) return "in " + (-elapsed).to_string();
        else if (elapsed.in_ns() == 0) return "now";
        else return elapsed.to_string() + " ago";
    }

    std::string to_string_exact() const {
        Duration elapsed = Instant::now() - *this;

        if (elapsed.in_ns() < 0) return "in " + (-elapsed).to_string_exact();
        else if (elapsed.in_ns() == 0) return "now";
        else return elapsed.to_string_exact() + " ago";
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

inline std::ostream& operator<<(std::ostream& os, const Instant& instant) {
    os << instant.to_string();
    return os;
}

} // namespace stu