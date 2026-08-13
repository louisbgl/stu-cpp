#include <catch2/catch_test_macros.hpp>
#include <thread>

#ifdef STU_SINGLE_HEADER_TEST
#include "stu.hpp"
#else
#include "instant.hpp"
#endif

using namespace stu;

TEST_CASE("Instant construction from now()", "[instant][construction]") {
    auto i1 = Instant::now();
    auto i2 = Instant::now();

    // Second instant should be >= first (time moves forward)
    REQUIRE(i2 >= i1);
}

TEST_CASE("Instant subtraction produces Duration", "[instant][arithmetic]") {
    auto start = Instant::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto end = Instant::now();

    auto elapsed = end - start;

    // Elapsed time should be positive and >= 10ms
    REQUIRE(elapsed.in_ms() >= 10.0);
}

TEST_CASE("Instant subtraction ordering", "[instant][arithmetic]") {
    auto i1 = Instant::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto i2 = Instant::now();

    auto forward = i2 - i1;   // positive duration
    auto backward = i1 - i2;  // negative duration

    REQUIRE(forward.in_ns() > 0);
    REQUIRE(backward.in_ns() < 0);
    REQUIRE(forward == -backward);
}

TEST_CASE("Instant addition with Duration", "[instant][arithmetic]") {
    auto start = Instant::now();
    auto offset = Duration::from_s(5);
    auto future = start + offset;

    auto diff = future - start;
    REQUIRE(diff == offset);
}

TEST_CASE("Instant subtraction with Duration", "[instant][arithmetic]") {
    auto start = Instant::now();
    auto offset = Duration::from_s(5);
    auto past = start - offset;

    auto diff = start - past;
    REQUIRE(diff == offset);
}

TEST_CASE("Instant addition with negative Duration", "[instant][arithmetic]") {
    auto start = Instant::now();
    auto offset = Duration::from_s(-5);
    auto past = start + offset;  // adding negative = going backwards

    REQUIRE(past < start);
    auto diff = start - past;
    REQUIRE(diff == -offset);
}

TEST_CASE("Instant subtraction with negative Duration", "[instant][arithmetic]") {
    auto start = Instant::now();
    auto offset = Duration::from_s(-5);
    auto future = start - offset;  // subtracting negative = going forwards

    REQUIRE(future > start);
    auto diff = future - start;
    REQUIRE(diff == -offset);
}

TEST_CASE("Instant addition overflow detection", "[instant][arithmetic][overflow]") {
    auto start = Instant::now();
    auto huge = Duration::from_ns(INT64_MAX / 2);

    // Adding huge duration twice should overflow
    auto once = start + huge;
    REQUIRE_THROWS_AS(once + huge, StuException);
}

TEST_CASE("Instant addition underflow detection", "[instant][arithmetic][overflow]") {
    auto start = Instant::now();
    auto step = Duration::from_ns(INT64_MIN / 10);

    bool threw = false;
    auto current = start;
    for (int i = 0; i < 11; ++i) {
        try {
            current = current + step;
        } catch (const StuException&) {
            threw = true;
            break;
        }
    }

    REQUIRE(threw);  // One of the additions must have underflowed
}

TEST_CASE("Instant subtraction underflow detection", "[instant][arithmetic][overflow]") {
    auto start = Instant::now();
    auto step = Duration::from_ns(INT64_MAX / 10);

    bool threw = false;
    auto current = start;
    for (int i = 0; i < 11; ++i) {
        try {
            current = current - step;
        } catch (const StuException&) {
            threw = true;
            break;
        }
    }

    REQUIRE(threw);  // One of the subtractions must have underflowed
}

TEST_CASE("Instant subtraction overflow detection", "[instant][arithmetic][overflow]") {
    auto start = Instant::now();
    auto huge_negative = Duration::from_ns(INT64_MIN / 2);

    // Subtracting huge negative duration twice should overflow
    auto once = start - huge_negative;
    REQUIRE_THROWS_AS(once - huge_negative, StuException);
}

TEST_CASE("Instant equality", "[instant][comparison]") {
    auto i1 = Instant::now();
    auto i2 = i1 + Duration::from_ns(0);  // same instant

    REQUIRE(i1 == i2);
    REQUIRE_FALSE(i1 != i2);
}

TEST_CASE("Instant inequality", "[instant][comparison]") {
    auto i1 = Instant::now();
    auto i2 = i1 + Duration::from_ns(1);

    REQUIRE(i1 != i2);
    REQUIRE_FALSE(i1 == i2);
}

TEST_CASE("Instant less than", "[instant][comparison]") {
    auto i1 = Instant::now();
    auto i2 = i1 + Duration::from_ms(1);

    REQUIRE(i1 < i2);
    REQUIRE_FALSE(i2 < i1);
    REQUIRE_FALSE(i1 < i1);
}

TEST_CASE("Instant less than or equal", "[instant][comparison]") {
    auto i1 = Instant::now();
    auto i2 = i1 + Duration::from_ms(1);
    auto i3 = i1;

    REQUIRE(i1 <= i2);
    REQUIRE(i1 <= i3);
    REQUIRE_FALSE(i2 <= i1);
}

TEST_CASE("Instant greater than", "[instant][comparison]") {
    auto i1 = Instant::now();
    auto i2 = i1 + Duration::from_ms(1);

    REQUIRE(i2 > i1);
    REQUIRE_FALSE(i1 > i2);
    REQUIRE_FALSE(i1 > i1);
}

TEST_CASE("Instant greater than or equal", "[instant][comparison]") {
    auto i1 = Instant::now();
    auto i2 = i1 + Duration::from_ms(1);
    auto i3 = i1;

    REQUIRE(i2 >= i1);
    REQUIRE(i1 >= i3);
    REQUIRE_FALSE(i1 >= i2);
}

TEST_CASE("Instant round-trip arithmetic", "[instant][arithmetic]") {
    auto start = Instant::now();
    auto offset = Duration::from_s(42);

    auto future = start + offset;
    auto back = future - offset;

    REQUIRE(back == start);
}

TEST_CASE("Instant monotonic guarantee", "[instant][edge-cases]") {
    // Successive calls to now() should never go backwards
    auto i1 = Instant::now();
    auto i2 = Instant::now();
    auto i3 = Instant::now();

    REQUIRE(i2 >= i1);
    REQUIRE(i3 >= i2);
}

TEST_CASE("Instant ordering transitivity", "[instant][comparison]") {
    auto i1 = Instant::now();
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    auto i2 = Instant::now();
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    auto i3 = Instant::now();

    // If i1 < i2 and i2 < i3, then i1 < i3
    if (i1 < i2 && i2 < i3) {
        REQUIRE(i1 < i3);
    }
}
