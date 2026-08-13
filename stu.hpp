// stu - Simple Timing Utils
#pragma once

#include <chrono>
#include <cstdint>
#include <exception>
#include <format>
#include <iostream>
#include <string>

namespace stu {

/**
 * @brief Exception class for errors in the stu library.
 */
class StuException : public std::exception {
public:
    explicit StuException(const std::string& message)
        : _message("[STU] " + message) {}

    explicit StuException(const std::string& file, int line, const std::string& message)
        : _message("[STU] " + file + ":" + std::to_string(line) + ": " + message) {}

    const char* what() const noexcept override {
        return _message.c_str();
    }

private:
    std::string _message;
};

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

    /*
     * @brief Create a Duration from a number of microseconds.
     * @param microseconds The number of microseconds to create the Duration from.
     * @return A Duration object representing the specified number of microseconds.
     * @throws stu::StuException if the resulting duration would overflow or underflow. 
     */
    static Duration from_us(int64_t microseconds) {
        return from_unit_checked(microseconds, _ONE_US_IN_NS, "us");
    }

    /*
     * @brief Create a Duration from a number of milliseconds.
     * @param milliseconds The number of milliseconds to create the Duration from.
     * @return A Duration object representing the specified number of milliseconds.
     * @throws stu::StuException if the resulting duration would overflow or underflow. 
     */
    static Duration from_ms(int64_t milliseconds) {
        return from_unit_checked(milliseconds, _ONE_MS_IN_NS, "ms");
    }

    /*
     * @brief Create a Duration from a number of seconds.
     * @param seconds The number of seconds to create the Duration from.
     * @return A Duration object representing the specified number of seconds.
     * @throws stu::StuException if the resulting duration would overflow or underflow. 
     */
    static Duration from_s(int64_t seconds) {
        return from_unit_checked(seconds, _ONE_S_IN_NS, "s");
    }

    /*
     * @brief Create a Duration from a number of minutes.
     * @param minutes The number of minutes to create the Duration from.
     * @return A Duration object representing the specified number of minutes.
     * @throws stu::StuException if the resulting duration would overflow or underflow. 
     */
    static Duration from_min(int64_t minutes) {
        return from_unit_checked(minutes, _ONE_MIN_IN_NS, "min");
    }

    /*
     * @brief Create a Duration from a number of hours.
     * @param hours The number of hours to create the Duration from.
     * @return A Duration object representing the specified number of hours.
     * @throws stu::StuException if the resulting duration would overflow or underflow. 
     */
    static Duration from_h(int64_t hours) {
        return from_unit_checked(hours, _ONE_H_IN_NS, "h");
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
        
        if (result.empty()) result = "0ns";
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

    static Duration from_unit_checked(int64_t value, int64_t multiplier, const char* unit_name) {
        if (value > INT64_MAX / multiplier) throw StuException("Duration::from_" + std::string(unit_name) + " overflow: " + std::to_string(value));
        if (value < INT64_MIN / multiplier) throw StuException("Duration::from_" + std::string(unit_name) + " underflow: " + std::to_string(value));
        return Duration::from_ns(value * multiplier);
    }
};

inline std::ostream& operator<<(std::ostream& os, const Duration& d) {
    os << d.to_string();
    return os;
}

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
