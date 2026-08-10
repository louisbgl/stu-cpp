#pragma once

#include <string>
#include <format>
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

    int64_t in_ns() const {
        return _nanoseconds;
    }

    int64_t in_us() const {
        return _nanoseconds / _ONE_US_IN_NS;
    }

    int64_t in_ms() const {
        return _nanoseconds / _ONE_MS_IN_NS;
    }

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