#include "src/stu.hpp"

int main() {
    stu::Duration d1 = stu::Duration::from_ns(20);
    std::cout << "Duration d1: " << d1 << std::endl;

    stu::Duration d2 = stu::Duration::from_us(30);
    std::cout << "Duration d2: " << d2 << std::endl << std::endl;
    
    std::cout << "Duration d1 + d2: " << d1 + d2 << std::endl;
    std::cout << "Duration d1 + d2 (exact mode): " << (d1 + d2).to_string_exact() << std::endl << std::endl;

    std::cout << "Duration d2 - d1: " << d2 - d1 << std::endl;
    std::cout << "Duration d2 - d1 (exact mode): " << (d2 - d1).to_string_exact() << std::endl << std::endl;

    std::cout << "Duration d1 * 100: " << d1 * 100 << std::endl;
    std::cout << "Duration d1 * 100 (exact mode): " << (d1 * 100).to_string_exact() << std::endl << std::endl;

    std::cout << "Duration d2 / 100: " << d2 / 100 << std::endl;
    std::cout << "Duration d2 / 100 (exact mode): " << (d2 / 100).to_string_exact() << std::endl << std::endl;

    stu::Duration d3 = stu::Duration::from_ms(123456789);
    std::cout << "Duration d3: " << d3 << std::endl;
    std::cout << "Duration d3 (exact mode): " << d3.to_string_exact() << std::endl;
    return 0;
}