#pragma once

#include <string>
#include <format>
#include <cstdint>
#include <iostream>

#include "exception.hpp"

namespace stu {

class Duration {
public:
    Duration() = default;
    ~Duration() = default;

    // Create a Duration from a number of nanoseconds
    static Duration from_ns(int64_t nanoseconds) {
        Duration d;
        d._nanoseconds = nanoseconds;
        return d;
    }

    // Create a Duration from a number of microseconds
    static Duration from_us(int64_t microseconds) {
        return Duration::from_ns(microseconds * _ONE_US_IN_NS);
    }

    // Create a Duration from a number of milliseconds
    static Duration from_ms(int64_t milliseconds) {
        return Duration::from_ns(milliseconds * _ONE_MS_IN_NS);
    }

    // Create a Duration from a number of seconds
    static Duration from_s(int64_t seconds) {
        return Duration::from_ns(seconds * _ONE_S_IN_NS);
    }

    // Create a Duration from a number of minutes
    static Duration from_min(int64_t minutes) {
        return Duration::from_ns(minutes * _ONE_MIN_IN_NS);
    }

    // Create a Duration from a number of hours
    static Duration from_h(int64_t hours) {
        return Duration::from_ns(hours * _ONE_H_IN_NS);
    }

    // Get the duration in nanoseconds
    int64_t in_ns() const {
        return _nanoseconds;
    }

    // Get the duration in microseconds
    double in_us() const {
        return static_cast<double>(_nanoseconds) / _ONE_US_IN_NS;
    }

    double in_ms() const {
        return static_cast<double>(_nanoseconds) / _ONE_MS_IN_NS;
    }

    // Get the duration in seconds
    double in_s() const {
        return static_cast<double>(_nanoseconds) / _ONE_S_IN_NS;
    }

    // Get the duration in minutes
    double in_min() const {
        return static_cast<double>(_nanoseconds) / _ONE_MIN_IN_NS;
    }

    // Get the duration in hours
    double in_h() const {
        return static_cast<double>(_nanoseconds) / _ONE_H_IN_NS;
    }

    /*
     * @brief Convert the duration to a human-readable string representation.
     * The string representation will use the largest appropriate time unit (h, min, s, ms, us, ns) and will include up to three decimal places for fractional values.
     * For example: 1 hour, 30 minutes will be represented as "1.5h"
     */
    std::string to_string() const {
        std::string result = "";
        int64_t abs_ns = _nanoseconds >= 0 ? _nanoseconds : -_nanoseconds;
        for (const auto& unit : _UNITS) {
            if (abs_ns >= unit.threshold_ns) {
                int64_t whole = abs_ns / unit.divisor_ns;
                result = std::to_string(whole);

                int64_t frac = abs_ns % unit.divisor_ns;
                int64_t frac_scaled = frac * 1000 / unit.divisor_ns;
                std::string frac_str = std::format("{:03d}", frac_scaled);

                while (!frac_str.empty() && frac_str.back() == '0') frac_str.pop_back();
                if (!frac_str.empty()) result += "." + frac_str;
                result += unit.suffix;
                break;
            }
        }

        if (_nanoseconds < 0) result = "-" + result;
        return result;
    }

    /*
     * @brief Convert the duration to a human-readable string representation in exact mode.
     * The string representation will include all time units (h, min, s, ms, us, ns) that are non-zero, separated by spaces.
     * For example: 1 hour, 30 minutes, 15 seconds will be represented as "1h 30min 15s"
     */
    std::string to_string_exact() const {
        std::string result = "";
        int64_t remaining = _nanoseconds >= 0 ? _nanoseconds : -_nanoseconds;
        for (const auto& unit : _UNITS) {
            int64_t whole = remaining / unit.divisor_ns;
            if (whole == 0) continue;

            result += std::to_string(whole) + unit.suffix + " ";
            remaining -= whole * unit.divisor_ns;
        }
        
        if (result.empty()) result = "0 ns";
        if (_nanoseconds < 0) result = "-" + result;
        return result;
    }

    // Throws stu::StuException on overlow or underflow
    Duration operator+(const Duration& other) const {
        // a + b > INT64_MAX equivalent to a > INT64_MAX - b
        // positive case
        if (_nanoseconds > 0 && other._nanoseconds > 0 && _nanoseconds > INT64_MAX - other._nanoseconds) {
            throw StuException("Duration addition overflow: " + std::to_string(_nanoseconds) + " + " + std::to_string(other._nanoseconds));
        }

        // negative case
        if (_nanoseconds < 0 && other._nanoseconds < 0 && _nanoseconds < INT64_MIN - other._nanoseconds) {
            throw StuException("Duration addition underflow: " + std::to_string(_nanoseconds) + " + " + std::to_string(other._nanoseconds));
        }

        Duration result;
        result._nanoseconds = _nanoseconds + other._nanoseconds;
        return result;
    }

    // Throws stu::StuException on overlow or underflow
    Duration operator-(const Duration& other) const {
        // if a positive and b negative, a - b > INT64_MAX equivalent to a > INT64_MAX + b
        if (_nanoseconds > 0 && other._nanoseconds < 0 && _nanoseconds > INT64_MAX + other._nanoseconds) {
            throw StuException("Duration subtraction overflow: " + std::to_string(_nanoseconds) + " - " + std::to_string(other._nanoseconds));
        }

        // if a negative and b positive, a - b < INT64_MIN equivalent to a < INT64_MIN + b
        if (_nanoseconds < 0 && other._nanoseconds > 0 && _nanoseconds < INT64_MIN + other._nanoseconds) {
            throw StuException("Duration subtraction underflow: " + std::to_string(_nanoseconds) + " - " + std::to_string(other._nanoseconds));
        }

        Duration result;
        result._nanoseconds = _nanoseconds - other._nanoseconds;
        return result;
    }

    // Throws stu::StuException on overlow or underflow
    Duration operator*(int64_t scalar) const {
        if (scalar == 0) return Duration::from_ns(0);

        // a * b > INT64_MAX equivalent to a > INT64_MAX / b
        if (scalar > 0 && (_nanoseconds > INT64_MAX / scalar || _nanoseconds < INT64_MIN / scalar)) {
            throw StuException("Duration multiplication overflow: " + std::to_string(_nanoseconds) + " * " + std::to_string(scalar));
        }

        // negative scalar division flips bounds
        if (scalar < 0 && (_nanoseconds > INT64_MIN / scalar || _nanoseconds < INT64_MAX / scalar)) {
            throw StuException("Duration multiplication overflow: " + std::to_string(_nanoseconds) + " * " + std::to_string(scalar));
        }

        Duration result;
        result._nanoseconds = _nanoseconds * scalar;
        return result;
    }

    // Throws stu::StuException on division by zero
    Duration operator/(int64_t scalar) const {
        if (scalar == 0) throw StuException("Duration division by zero");

        Duration result;
        result._nanoseconds = _nanoseconds / scalar;
        return result;
    }

    Duration operator-() const {
        Duration result;
        result._nanoseconds = -_nanoseconds;
        return result;
    }

    bool operator==(const Duration& other) const {
        return _nanoseconds == other._nanoseconds;
    }

    bool operator!=(const Duration& other) const {
        return _nanoseconds != other._nanoseconds;
    }

    bool operator<(const Duration& other) const {
        return _nanoseconds < other._nanoseconds;
    }

    bool operator<=(const Duration& other) const {
        return _nanoseconds <= other._nanoseconds;
    }

    bool operator>(const Duration& other) const {
        return _nanoseconds > other._nanoseconds;
    }

    bool operator>=(const Duration& other) const {
        return _nanoseconds >= other._nanoseconds;
    }

private:
    int64_t _nanoseconds = 0; // gives a max of ~292 years (negative or positive)

    constexpr static int64_t _ONE_US_IN_NS = 1000;
    constexpr static int64_t _ONE_MS_IN_NS = 1000 * _ONE_US_IN_NS;
    constexpr static int64_t _ONE_S_IN_NS = 1000 * _ONE_MS_IN_NS;
    constexpr static int64_t _ONE_MIN_IN_NS = 60 * _ONE_S_IN_NS;
    constexpr static int64_t _ONE_H_IN_NS = 60 * _ONE_MIN_IN_NS;
    
    struct Unit {
        int64_t threshold_ns;
        int64_t divisor_ns;
        const char* suffix;
    };

    constexpr static Unit _UNITS[] = {
        { _ONE_H_IN_NS,   _ONE_H_IN_NS,   "h" },
        { _ONE_MIN_IN_NS, _ONE_MIN_IN_NS, "min" },
        { _ONE_S_IN_NS,   _ONE_S_IN_NS,   "s" },
        { _ONE_MS_IN_NS,  _ONE_MS_IN_NS,  "ms" },
        { _ONE_US_IN_NS,  _ONE_US_IN_NS,  "us" },
        { 0,             1,               "ns" } // fallback
    };
};

inline std::ostream& operator<<(std::ostream& os, const Duration& d) {
    os << d.to_string();
    return os;
}

}