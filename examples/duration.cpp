#include <iostream>

#include "stu.hpp"

// The Duration class is straightforward.
// It represents a time span with nanosecond precision.
// Can be positive, and also negative, and zero obviously.
// It can represent time spans of up to ~292 years (positive or negative).

// Duration is a type returned by mathematical operations on Instants and Timestamp objects.

// Want the full public API for Duration ? it's in README.md :)

int main() {
    // You can create a Duration from nanoseconds as so:
    stu::Duration d1 = stu::Duration::from_ns(20);

    // You can also create a Duration from other units, such as seconds:
    // There exists, from_ns, from_us, from_ms, from_s, from_min, and from_h.
    stu::Duration d2 = stu::Duration::from_s(5);

    // You can add Durations together:
    stu::Duration d3 = d1 + d2;

    // You can also subtract Durations:
    stu::Duration d4 = d2 - d1;

    // Let's print these Durations out like so:
    std::cout << "d1: " << d1 << std::endl;
    std::cout << "d2: " << d2 << std::endl;
    std::cout << "d3: " << d3 << std::endl;

    // You can also use Duration.to_string() (returns a string, which is what the << operator uses)
    std::cout << "d4: " << d4.to_string() << std::endl;

    // Additionally, you can use to_string_exact() to get a full representation of the duration in all units (h, min, s, ms, us, ns).
    stu::Duration d5 = stu::Duration::from_h(1) + stu::Duration::from_min(30) + stu::Duration::from_s(15);
    std::cout << "d5: " << d5.to_string_exact() << std::endl; // This will print "1h 30min 15s"

    // Now, say you want to know a Duration in a specific unit, you can do:
    // There exists in_ns(), in_us(), in_ms(), in_s(), in_min(), and in_h() for this purpose.
    std::cout << "d5 in seconds: " << d5.in_s() << "s" << std::endl;

    // Finally, you can compare Durations, obviously:
    if (d1 < d2) { // always true (20ns < 5s)
        std::cout << "d1 is shorter than d2" << std::endl;
    }

    // You can use <, <=, >, >=, and also == and != for comparisons.
    // Note all comparisons compare to the nanosecond, so 1s == 1000ms is true, for example.

    // I'll also note here that you can represent Durations way larger than hours,
    // tho i don't provide anything for days, weeks, months, or years.
    // This is because to be accurate in these units, a Duration would have to be calendar aware, which beats the simple ideology of this class.

    // If you want to create a Duration of 1 year, you can do:
    stu::Duration one_year = stu::Duration::from_h(24 * 365);
    // It's not perfect, but it's a simple approximation.

    return 0;
}