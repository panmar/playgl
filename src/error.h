#pragma once

#include <stdexcept>
#include <string>

class PlayGlException : public std::runtime_error {
public:
    PlayGlException(const std::string& message) : std::runtime_error(message) {
        
    }
};