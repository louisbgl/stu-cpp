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
        return Duration::from_ns(microseconds * 1000);
    }

    static Duration from_ms(int64_t milliseconds) {
        return Duration::from_ns(milliseconds * 1000 * 1000);
    }

    static Duration from_s(int64_t seconds) {
        return Duration::from_ns(seconds * 1000 * 1000 * 1000);
    }

    static Duration from_min(int64_t minutes) {
        return Duration::from_ns(minutes * 60 * 1000 * 1000 * 1000);
    }

    static Duration from_h(int64_t hours) {
        return Duration::from_ns(hours * 60 * 60 * 1000 * 1000 * 1000);
    }

    std::string to_string() const {
        return std::to_string(_nanoseconds) + " ns"; // TODO improve
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
};

inline std::ostream& operator<<(std::ostream& os, const Duration& d) {
    os << d.to_string();
    return os;
}

}