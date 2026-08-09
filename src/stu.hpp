#pragma once

#include <string>
#include <iostream>

namespace stu {

class Duration {
public:
    Duration() = default;
    ~Duration() = default;

    static Duration from_ns(int64_t nanoseconds) {
        Duration d;
        d._nanoseconds = nanoseconds;
        return d;
    }

    static Duration from_us(int64_t microseconds) {
        return Duration::from_ns(microseconds * _ONE_US_IN_NS);
    }

    static Duration from_ms(int64_t milliseconds) {
        return Duration::from_ns(milliseconds * _ONE_MS_IN_NS);
    }

    static Duration from_s(int64_t seconds) {
        return Duration::from_ns(seconds * _ONE_S_IN_NS);
    }

    static Duration from_min(int64_t minutes) {
        return Duration::from_ns(minutes * _ONE_MIN_IN_NS);
    }

    static Duration from_h(int64_t hours) {
        return Duration::from_ns(hours * _ONE_H_IN_NS);
    }

    std::string to_string() const {
        if      (_nanoseconds < _ONE_US_IN_NS)  return std::to_string(_nanoseconds) + " ns";
        else if (_nanoseconds < _ONE_MS_IN_NS)  return std::to_string(_nanoseconds / _ONE_US_IN_NS) + " us";
        else if (_nanoseconds < _ONE_S_IN_NS)   return std::to_string(_nanoseconds / _ONE_MS_IN_NS) + " ms";
        else if (_nanoseconds < _ONE_MIN_IN_NS) return std::to_string(_nanoseconds / _ONE_S_IN_NS) + " s";
        else if (_nanoseconds < _ONE_H_IN_NS)   return std::to_string(_nanoseconds / _ONE_MIN_IN_NS) + " min";
        else return "does this even happen? " + std::to_string(_nanoseconds) + " ns";
    }

    Duration operator+(const Duration& other) const {
        Duration result;
        result._nanoseconds = _nanoseconds + other._nanoseconds;
        return result;
    }

    Duration operator-(const Duration& other) const {
        Duration result;
        result._nanoseconds = _nanoseconds - other._nanoseconds;
        return result;
    }

    Duration operator*(const Duration& other) const {
        Duration result;
        result._nanoseconds = _nanoseconds * other._nanoseconds;
        return result;
    }

    Duration operator/(const Duration& other) const {
        Duration result;
        result._nanoseconds = _nanoseconds / other._nanoseconds;
        return result;
    }

    Duration operator*(int64_t scalar) const {
        Duration result;
        result._nanoseconds = _nanoseconds * scalar;
        return result;
    }

    Duration operator/(int64_t scalar) const {
        Duration result;
        result._nanoseconds = _nanoseconds / scalar;
        return result;
    }

private:
    int64_t _nanoseconds = 0; // gives a max of ~292 years (negative or positive)

    constexpr static int64_t _ONE_US_IN_NS = 1000;
    constexpr static int64_t _ONE_MS_IN_NS = 1000 * _ONE_US_IN_NS;
    constexpr static int64_t _ONE_S_IN_NS = 1000 * _ONE_MS_IN_NS;
    constexpr static int64_t _ONE_MIN_IN_NS = 60 * _ONE_S_IN_NS;
    constexpr static int64_t _ONE_H_IN_NS = 60 * _ONE_MIN_IN_NS;
};

inline std::ostream& operator<<(std::ostream& os, const Duration& d) {
    os << d.to_string();
    return os;
}

}