#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#ifdef STU_SINGLE_HEADER_TEST
#include "stu.hpp"
#else
#include "duration.hpp"
#endif

using namespace stu;

TEST_CASE("Duration construction from nanoseconds", "[duration][construction]") {
    auto d = Duration::from_ns(1500);
    REQUIRE(d.in_ns() == 1500);
}

TEST_CASE("Duration construction from microseconds", "[duration][construction]") {
    auto d = Duration::from_us(250);
    REQUIRE(d.in_ns() == 250000);
}

TEST_CASE("Duration construction from milliseconds", "[duration][construction]") {
    auto d = Duration::from_ms(100);
    REQUIRE(d.in_ns() == 100000000);
}

TEST_CASE("Duration construction from seconds", "[duration][construction]") {
    auto d = Duration::from_s(5);
    REQUIRE(d.in_ns() == 5000000000);
}

TEST_CASE("Duration construction from minutes", "[duration][construction]") {
    auto d = Duration::from_min(2);
    REQUIRE(d.in_ns() == 120000000000);
}

TEST_CASE("Duration construction from hours", "[duration][construction]") {
    auto d = Duration::from_h(1);
    REQUIRE(d.in_ns() == 3600000000000);
}

TEST_CASE("Duration construction overflow detection", "[duration][construction][overflow]") {
    REQUIRE_THROWS_AS(Duration::from_us(INT64_MAX), StuException);
    REQUIRE_THROWS_AS(Duration::from_ms(INT64_MAX), StuException);
    REQUIRE_THROWS_AS(Duration::from_s(INT64_MAX), StuException);
    REQUIRE_THROWS_AS(Duration::from_min(INT64_MAX), StuException);
    REQUIRE_THROWS_AS(Duration::from_h(INT64_MAX), StuException);
}

TEST_CASE("Duration construction underflow detection", "[duration][construction][overflow]") {
    REQUIRE_THROWS_AS(Duration::from_us(INT64_MIN), StuException);
    REQUIRE_THROWS_AS(Duration::from_ms(INT64_MIN), StuException);
    REQUIRE_THROWS_AS(Duration::from_s(INT64_MIN), StuException);
    REQUIRE_THROWS_AS(Duration::from_min(INT64_MIN), StuException);
    REQUIRE_THROWS_AS(Duration::from_h(INT64_MIN), StuException);
}

TEST_CASE("Duration conversion to fractional units", "[duration][conversion]") {
    auto d = Duration::from_ns(1234567);

    REQUIRE(d.in_ns() == 1234567);
    REQUIRE_THAT(d.in_us(), Catch::Matchers::WithinRel(1234.567, 0.001));
    REQUIRE_THAT(d.in_ms(), Catch::Matchers::WithinRel(1.234567, 0.000001));
    REQUIRE_THAT(d.in_s(), Catch::Matchers::WithinRel(0.001234567, 0.00000001));
}

TEST_CASE("Duration zero", "[duration][edge-cases]") {
    auto d = Duration::from_ns(0);

    REQUIRE(d.in_ns() == 0);
    REQUIRE(d.in_us() == 0.0);
    REQUIRE(d.to_string() == "0ns");
    REQUIRE(d.to_string_exact() == "0ns");
}

TEST_CASE("Duration negative values", "[duration][edge-cases]") {
    auto d = Duration::from_ns(-5000);

    REQUIRE(d.in_ns() == -5000);
    REQUIRE(d.to_string() == "-5us");
}

TEST_CASE("Duration to_string auto-scaling", "[duration][string]") {
    REQUIRE(Duration::from_ns(500).to_string() == "500ns");
    REQUIRE(Duration::from_ns(1000).to_string() == "1us");  // boundary promotion
    REQUIRE(Duration::from_ns(1234).to_string() == "1.234us");
    REQUIRE(Duration::from_ns(1200).to_string() == "1.2us");  // trailing zeros trimmed
    REQUIRE(Duration::from_us(1500).to_string() == "1.5ms");
    REQUIRE(Duration::from_ms(1500).to_string() == "1.5s");
    REQUIRE(Duration::from_s(90).to_string() == "1.5min");
    REQUIRE(Duration::from_min(90).to_string() == "1.5h");
}

TEST_CASE("Duration to_string_exact breakdown", "[duration][string]") {
    auto d = Duration::from_h(1) + Duration::from_min(23) + Duration::from_s(4) + Duration::from_ms(567);
    REQUIRE(d.to_string_exact() == "1h 23min 4s 567ms ");

    auto d2 = Duration::from_ns(1234);
    REQUIRE(d2.to_string_exact() == "1us 234ns ");
}

TEST_CASE("Duration addition", "[duration][arithmetic]") {
    auto d1 = Duration::from_ms(100);
    auto d2 = Duration::from_ms(50);
    auto sum = d1 + d2;

    REQUIRE(sum.in_ms() == 150.0);
}

TEST_CASE("Duration addition overflow", "[duration][arithmetic][overflow]") {
    auto d1 = Duration::from_ns(INT64_MAX - 100);
    auto d2 = Duration::from_ns(200);

    REQUIRE_THROWS_AS(d1 + d2, StuException);
}

TEST_CASE("Duration addition underflow", "[duration][arithmetic][overflow]") {
    auto d1 = Duration::from_ns(INT64_MIN + 100);
    auto d2 = Duration::from_ns(-200);

    REQUIRE_THROWS_AS(d1 + d2, StuException);
}

TEST_CASE("Duration subtraction", "[duration][arithmetic]") {
    auto d1 = Duration::from_ms(100);
    auto d2 = Duration::from_ms(30);
    auto diff = d1 - d2;

    REQUIRE(diff.in_ms() == 70.0);
}

TEST_CASE("Duration subtraction overflow", "[duration][arithmetic][overflow]") {
    auto d1 = Duration::from_ns(INT64_MAX - 100);
    auto d2 = Duration::from_ns(-200);

    REQUIRE_THROWS_AS(d1 - d2, StuException);
}

TEST_CASE("Duration subtraction underflow", "[duration][arithmetic][overflow]") {
    auto d1 = Duration::from_ns(INT64_MIN + 100);
    auto d2 = Duration::from_ns(200);

    REQUIRE_THROWS_AS(d1 - d2, StuException);
}

TEST_CASE("Duration scalar multiplication", "[duration][arithmetic]") {
    auto d = Duration::from_ms(100);
    auto scaled = d * 3;

    REQUIRE(scaled.in_ms() == 300.0);
}

TEST_CASE("Duration scalar multiplication by zero", "[duration][arithmetic]") {
    auto d = Duration::from_ms(100);
    auto result = d * 0;

    REQUIRE(result.in_ns() == 0);
}

TEST_CASE("Duration scalar multiplication overflow", "[duration][arithmetic][overflow]") {
    auto d = Duration::from_ns(INT64_MAX / 2);

    REQUIRE_THROWS_AS(d * 3, StuException);
}

TEST_CASE("Duration scalar multiplication negative scalar", "[duration][arithmetic]") {
    auto d = Duration::from_ms(100);
    auto result = d * -2;

    REQUIRE(result.in_ms() == -200.0);
}

TEST_CASE("Duration scalar division", "[duration][arithmetic]") {
    auto d = Duration::from_ms(300);
    auto halved = d / 2;

    REQUIRE(halved.in_ms() == 150.0);
}

TEST_CASE("Duration division by zero", "[duration][arithmetic][overflow]") {
    auto d = Duration::from_ms(100);

    REQUIRE_THROWS_AS(d / 0, StuException);
}

TEST_CASE("Duration unary negation", "[duration][arithmetic]") {
    auto d = Duration::from_ms(100);
    auto neg = -d;

    REQUIRE(neg.in_ms() == -100.0);

    auto neg_neg = -neg;
    REQUIRE(neg_neg.in_ms() == 100.0);
}

TEST_CASE("Duration equality", "[duration][comparison]") {
    auto d1 = Duration::from_ms(100);
    auto d2 = Duration::from_ms(100);
    auto d3 = Duration::from_ms(99);

    REQUIRE(d1 == d2);
    REQUIRE_FALSE(d1 == d3);
}

TEST_CASE("Duration inequality", "[duration][comparison]") {
    auto d1 = Duration::from_ms(100);
    auto d2 = Duration::from_ms(99);

    REQUIRE(d1 != d2);
    REQUIRE_FALSE(d1 != d1);
}

TEST_CASE("Duration less than", "[duration][comparison]") {
    auto d1 = Duration::from_ms(50);
    auto d2 = Duration::from_ms(100);

    REQUIRE(d1 < d2);
    REQUIRE_FALSE(d2 < d1);
    REQUIRE_FALSE(d1 < d1);
}

TEST_CASE("Duration less than or equal", "[duration][comparison]") {
    auto d1 = Duration::from_ms(50);
    auto d2 = Duration::from_ms(100);
    auto d3 = Duration::from_ms(100);

    REQUIRE(d1 <= d2);
    REQUIRE(d2 <= d3);
    REQUIRE_FALSE(d2 <= d1);
}

TEST_CASE("Duration greater than", "[duration][comparison]") {
    auto d1 = Duration::from_ms(100);
    auto d2 = Duration::from_ms(50);

    REQUIRE(d1 > d2);
    REQUIRE_FALSE(d2 > d1);
    REQUIRE_FALSE(d1 > d1);
}

TEST_CASE("Duration greater than or equal", "[duration][comparison]") {
    auto d1 = Duration::from_ms(100);
    auto d2 = Duration::from_ms(50);
    auto d3 = Duration::from_ms(100);

    REQUIRE(d1 >= d2);
    REQUIRE(d1 >= d3);
    REQUIRE_FALSE(d2 >= d1);
}

TEST_CASE("Duration stream output", "[duration][string]") {
    auto d = Duration::from_ms(1234);
    std::ostringstream oss;
    oss << d;

    REQUIRE(oss.str() == "1.234s");
}

TEST_CASE("Duration max representable value", "[duration][edge-cases]") {
    // ~292 years in nanoseconds
    auto d = Duration::from_ns(INT64_MAX);

    REQUIRE(d.in_ns() == INT64_MAX);
    REQUIRE(d.in_h() > 2.5e6);  // roughly 292 years = ~2.56M hours
}

TEST_CASE("Duration threshold boundary exact promotion", "[duration][string]") {
    // Exactly at threshold should promote to next unit
    REQUIRE(Duration::from_ns(1000).to_string() == "1us");
    REQUIRE(Duration::from_us(1000).to_string() == "1ms");
    REQUIRE(Duration::from_ms(1000).to_string() == "1s");
    REQUIRE(Duration::from_s(60).to_string() == "1min");
    REQUIRE(Duration::from_min(60).to_string() == "1h");
}

TEST_CASE("Duration precision in to_string", "[duration][string]") {
    // Max 3 decimal digits
    auto d = Duration::from_ns(1234567);  // 1.234567 ms
    REQUIRE(d.to_string() == "1.234ms");  // truncated to 3 digits

    // Trailing zeros trimmed
    auto d2 = Duration::from_ns(1000000);  // exactly 1 ms
    REQUIRE(d2.to_string() == "1ms");  // no ".000"
}
