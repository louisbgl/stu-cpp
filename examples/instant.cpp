#include "instant.hpp"
#include "stu.hpp"

#include <iostream>
#include <thread>

int main() {
    stu::Instant now = stu::Instant::now();

    // simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(1));

    stu::Instant later = stu::Instant::now();

    stu::Duration elapsed = later - now;
    std::cout << "Elapsed time: " << elapsed << std::endl;

    stu::Duration two_seconds = stu::Duration::from_s(2);
    stu::Instant future = now + two_seconds;
    // cant print instant yet...

    stu::Instant past = future - two_seconds; // should be equal to now (almost, i suppose)


    return 0;
}