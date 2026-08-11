# stu, Simple Timing Utils

A modern C++20 header-only library for time handling with clear semantics and type safety.

## Requirements

- C++20 or later

## Features

- **Header-only**: zero dependencies
- **Duration**: represents a timespan, with nanosecond precision
- **Duration arithmetic**: add, subtract, multiply by scalars, compare Durations
- **Duration pretty-printing**: Auto-scaling (`"1.234 ms"`) or exact breakdown (`"1h 23min 4s"`)

## Status

**Early development**: `Duration` mostly complete, `Instant` and `Timestamp` not yet implemented.

## Installation

stu ships as a single header. You only need ```stu.hpp```.

### Option 1: Download the header
```sh
curl -O https://raw.githubusercontent.com/louisbgl/stu-cpp/main/stu.hpp
```
```cpp
#include "stu.hpp"
```

### Option 2: CMake FetchContent
```cmake
include(FetchContent)
FetchContent_Declare(stu
  GIT_REPOSITORY https://github.com/louisbgl/stu-cpp.git
  GIT_TAG main)
FetchContent_MakeAvailable(stu)

target_link_libraries(your_app PRIVATE stu)
```
```cpp
#include "stu.hpp"
```

## Examples

Build and run examples with CMake from the project root:

```bash
cmake -S . -B build && cmake --build build

cmake --build build --target run_duration
```

See [`examples/duration.cpp`](examples/duration.cpp) for a walkthrough of Duration usage.

## Building & Testing

Requires CMake 3.15+ and a C++20 compiler.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## API Reference

### Duration

**Construction:**
```cpp
auto d = stu::Duration::from_ns(1500);
auto d = stu::Duration::from_us(250);
auto d = stu::Duration::from_ms(100);
auto d = stu::Duration::from_s(5);
auto d = stu::Duration::from_min(2);
auto d = stu::Duration::from_h(1);
// All throw stu::StuException on overflow
```

**Conversion:**
```cpp
int64_t ns = d.in_ns();   // exact nanoseconds
double us  = d.in_us();   // fractional microseconds
double ms  = d.in_ms();
double s   = d.in_s();
// ... in_min(), in_h() also available
```

**String representation:**
```cpp
std::string auto_scale = d.to_string();        // "1.234 ms" (auto-selects unit)
std::string exact = d.to_string_exact();       // "1h 23min 4s 567ms" (all units)
std::cout << d << "\n";                        // uses to_string()
```

**Arithmetic:**
```cpp
Duration sum  = d1 + d2;    // throws on overflow
Duration diff = d1 - d2;    // throws on overflow
Duration scaled = d * 10;   // multiply by scalar, throws on overflow
Duration halved = d / 2;    // divide by scalar, throws on division by zero
Duration neg = -d;          // unary negation
```

**Comparison:**
```cpp
// ==, !=, <, <=, >, >= all supported (nanosecond-exact)
if (d1 < d2) { ... }
```

## License

See [LICENSE.md](LICENSE.md)
