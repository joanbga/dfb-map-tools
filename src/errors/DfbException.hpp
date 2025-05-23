#pragma once

#include <stdexcept>

class DfbException : public std::exception {
   private:
    std::string message;

   public:
    DfbException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};