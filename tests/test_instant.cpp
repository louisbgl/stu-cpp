#include <catch2/catch_test_macros.hpp>
#include <sstream>
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

TEST_CASE("Instant to_string() for past instant", "[instant][formatting]") {
    auto past = Instant::now() - Duration::from_s(5);
    auto str = past.to_string();

    // Should end with " ago" and contain time unit
    REQUIRE(str.find(" ago") != std::string::npos);
    REQUIRE(str.find("s") != std::string::npos);
}

TEST_CASE("Instant to_string() for future instant", "[instant][formatting]") {
    auto future = Instant::now() + Duration::from_s(10);
    auto str = future.to_string();

    // Should start with "in " and contain time unit
    REQUIRE(str.find("in ") == 0);
    REQUIRE(str.find("s") != std::string::npos);
}

TEST_CASE("Instant to_string() for exactly now", "[instant][formatting]") {
    auto instant = Instant::now();
    auto str = instant.to_string();

    // Should be "now" (or close to it due to timing)
    // Accept both "now" and very small elapsed times (< 1ms)
    bool is_now_or_close = (str == "now") ||
                           (str.find("ns ago") != std::string::npos) ||
                           (str.find("us ago") != std::string::npos);
    REQUIRE(is_now_or_close);
}

TEST_CASE("Instant to_string_exact() for past instant", "[instant][formatting]") {
    auto past = Instant::now() - Duration::from_s(90);  // 1min 30s
    auto str = past.to_string_exact();

    // Should end with " ago" and contain multiple time units
    REQUIRE(str.find(" ago") != std::string::npos);
    REQUIRE(str.find("min") != std::string::npos);
}

TEST_CASE("Instant to_string_exact() for future instant", "[instant][formatting]") {
    auto future = Instant::now() + Duration::from_h(2) + Duration::from_min(30);
    auto str = future.to_string_exact();

    // Should start with "in " and contain multiple time units
    REQUIRE(str.find("in ") == 0);
    REQUIRE(str.find("h") != std::string::npos);
}

TEST_CASE("Instant to_string_exact() for exactly now", "[instant][formatting]") {
    auto instant = Instant::now();
    auto str = instant.to_string_exact();

    // Should be "now" (or close to it due to timing)
    bool is_now_or_close = (str == "now") ||
                           (str.find("ns ago") != std::string::npos) ||
                           (str.find("us ago") != std::string::npos);
    REQUIRE(is_now_or_close);
}

TEST_CASE("Instant operator<< uses to_string()", "[instant][formatting]") {
    auto past = Instant::now() - Duration::from_ms(500);

    std::ostringstream oss;
    oss << past;

    auto str = oss.str();
    REQUIRE(str.find(" ago") != std::string::npos);
}

TEST_CASE("Instant to_string() accuracy check", "[instant][formatting]") {
    // Create instant exactly 5 seconds in the past
    auto past = Instant::now() - Duration::from_s(5);

    // to_string() should show ~5s (allowing for execution time)
    auto str = past.to_string();

    // Should contain "5" and "s" and " ago"
    REQUIRE(str.find("5") != std::string::npos);
    REQUIRE(str.find("s") != std::string::npos);
    REQUIRE(str.find(" ago") != std::string::npos);
}

TEST_CASE("Instant to_string_exact() accuracy check", "[instant][formatting]") {
    // Create instant exactly 1h 23min 45s in the future
    auto future = Instant::now() + Duration::from_h(1) + Duration::from_min(23) + Duration::from_s(45);

    auto str = future.to_string_exact();

    // Should contain hour and minute components (seconds might drift due to execution time)
    REQUIRE(str.find("in ") == 0);
    REQUIRE(str.find("1h") != std::string::npos);
    REQUIRE(str.find("23min") != std::string::npos);
    // Accept either 45s or 44s due to execution time drift
    bool has_seconds = (str.find("45s") != std::string::npos) || (str.find("44s") != std::string::npos);
    REQUIRE(has_seconds);
}
