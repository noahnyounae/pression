#ifndef DECOMPRESSOR_HXX
#define DECOMPRESSOR_HXX

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <iostream>

// readFile: read entire file into string
inline std::string readFile(const std::string &filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }
    std::ostringstream oss;
    oss << infile.rdbuf();
    return oss.str();
}

// split: splits string by a delimiter
inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            tokens.push_back(item);
    }
    return tokens;
}

#endif // DECOMPRESSOR_HXX
