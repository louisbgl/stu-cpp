#pragma once

#include <exception>
#include <string>

namespace stu {

/**
 * @brief Exception class for errors in the stu library.
 */
class StuException : public std::exception {
public:
    explicit StuException(const std::string& message)
        : _message("[STU] " + message) {}

    explicit StuException(const std::string& file, int line, const std::string& message)
        : _message("[STU] " + file + ":" + std::to_string(line) + ": " + message) {}

    const char* what() const noexcept override {
        return _message.c_str();
    }

private:
    std::string _message;
};

} // namespace stu
