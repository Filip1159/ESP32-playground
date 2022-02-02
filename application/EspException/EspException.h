#pragma once

#include <exception>
#include <string>

using namespace std;

class EspException : public exception {
private:
    string message;

public:
    EspException() = default;
    EspException(const string& message) :message(message) {}

    const char* what() const throw () {
        return message.c_str();
    }
};