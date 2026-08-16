#include "stu.hpp"

#include <iostream>
#include <thread>

int main() {
    // TEMPORARY EXAMPLE
    stu::Instant now = stu::Instant::now();

    // simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(1));

    stu::Instant later = stu::Instant::now();

    stu::Duration elapsed = later - now;
    std::cout << "Elapsed time: " << elapsed << std::endl;

    stu::Duration two_seconds = stu::Duration::from_s(2);
    stu::Instant future = now + two_seconds;
    // cant print instant yet...

    stu::Instant computed_now = future - two_seconds; // Instant - 2s + 2s = Instant, as it should be
    if (computed_now == now) {
        std::cout << "Computed now is equal to now." << std::endl;
    }

    auto i1 = stu::Instant::now();
    auto i2 = i1 + stu::Duration::from_s(5);
    auto i3 = i1 - stu::Duration::from_s(5);
    std::cout << "Now: " << i1 << std::endl;
    std::cout << "Now exact: " << i1.to_string_exact() << std::endl;
    std::cout << "In 5s: " << i2 << std::endl;
    std::cout << "In 5s exact: " << i2.to_string_exact() << std::endl;
    std::cout << "5s ago: " << i3 << std::endl;
    std::cout << "5s ago exact: " << i3.to_string_exact() << std::endl;

    return 0;
}