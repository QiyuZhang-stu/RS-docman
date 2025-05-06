#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>

const std::string API_ENDPOINT{ "http://docman.zhuof.wang" };

inline std::string encodeUriComponent(const std::string& s) {
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (char c : s) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        }
        else if (c == ' ') {
            encoded << '+';
        }
        else {
            encoded << '%' << std::uppercase << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }

    return encoded.str();
}

inline std::string readFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string content(size, '\0');
    if (!file.read(&content[0], size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }
    return content;
}

#endif // UTILS_HPP
