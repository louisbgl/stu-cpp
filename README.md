# stu

A modern C++20 header-only library for time handling with clear semantics and type safety.

### The following is placeholder text tbh

## Requirements

- **C++20** or later
- **GCC 13+** / **Clang 16+** (for `std::format` support)

## Overview

**stu** provides three distinct types for different time-related use cases, avoiding the pitfalls of "one type does everything" approaches:

- **`Duration`** — A time span stored as nanoseconds. Supports full arithmetic, negative values, and pretty-printing.
- **`Instant`** — A monotonic point in time (steady_clock-backed) for elapsed-time measurement within a process.
- **`Timestamp`** — A wall-clock point in time (system_clock-backed) for calendar dates, logging, and persistence.

**Design philosophy:** No implicit conversions between `Instant` and `Timestamp` — they have fundamentally different guarantees. Compile-time errors prevent mixing incompatible time types.

## Features (Current)

- **Duration arithmetic** — Add, subtract, multiply by scalars, compare
- **Negative durations** — Fully supported
- **Pretty-printing** — Auto-scaling (`"1.234 ms"`) or exact breakdown (`"1h 23min 4s"`)
- **Header-only** — Drop `stu.hpp` into your project
- **Type-safe** — No accidental mixing of monotonic and wall-clock time

## Status

🚧 **Early development** — `Duration` mostly complete, `Instant` and `Timestamp` not yet implemented.

## Installation

**Direct include:**

```cpp
#include "stu.hpp"
```

**CMake:**

```cmake
# Copy stu.hpp to your project, then:
target_include_directories(your_target PRIVATE ${PROJECT_SOURCE_DIR}/include)
```

## Quick Example

```cpp
#include "stu.hpp"
#include <iostream>

int main() {
    auto d1 = stu::Duration::from_ms(1500);
    auto d2 = stu::Duration::from_us(250);

    std::cout << d1 + d2 << "\n";              // "1.50025 ms"
    std::cout << d1.to_string_exact() << "\n"; // "1s 500ms"

    auto elapsed = stu::Duration::from_h(2) + stu::Duration::from_min(15);
    std::cout << elapsed << "\n";              // "2.25 h"
    std::cout << elapsed.to_string_exact() << "\n"; // "2h 15min"

    return 0;
}
```

**Build:**

```bash
g++ -std=c++20 example.cpp -o example
./example
```

## API Reference (Duration)

### Construction

```cpp
static Duration from_ns(int64_t nanoseconds);
static Duration from_us(int64_t microseconds);
static Duration from_ms(int64_t milliseconds);
static Duration from_s(int64_t seconds);
static Duration from_min(int64_t minutes);
static Duration from_h(int64_t hours);
```

### Accessors

```cpp
int64_t in_ns() const;
int64_t in_us() const;
int64_t in_ms() const;
```

### String Conversion

```cpp
std::string to_string() const;        // Auto-scale to readable unit: "1.234 ms"
std::string to_string_exact() const;  // Exact breakdown: "1h 23min 4s 567ms"
```

### Operators

```cpp
Duration operator+(const Duration& other) const;
Duration operator-(const Duration& other) const;
Duration operator*(int64_t scalar) const;
Duration operator/(int64_t scalar) const;
Duration operator-() const;  // Unary negation

bool operator==(const Duration& other) const;
bool operator!=(const Duration& other) const;
bool operator<(const Duration& other) const;
bool operator<=(const Duration& other) const;
bool operator>(const Duration& other) const;
bool operator>=(const Duration& other) const;
```

### Stream Output

```cpp
std::ostream& operator<<(std::ostream& os, const Duration& d);
```

## Roadmap

- [ ] Overflow/underflow checks on arithmetic operators
- [ ] `Instant` implementation (monotonic time, benchmarking)
- [ ] `Timestamp` implementation (wall-clock time, calendar operations)
- [ ] String parsing (`Duration::parse("1h 30m")`)
- [ ] Integration with **cord** (config parsing)
- [ ] Timer/scoped-timer utilities

## License

See [LICENSE.md](LICENSE.md)

---

**stu** — A companion library to [cord](https://github.com/louisbgl/cord-cpp) for modern C++ projects.
