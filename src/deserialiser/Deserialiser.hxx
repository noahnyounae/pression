#ifndef DESERIALISER_HXX
#define DESERIALISER_HXX

#include "Deserialiser.hh"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <algorithm>

inline bool checkHeader(std::ifstream &infile, const char* expected) {
    char header[4];
    infile.read(header, 4);
    for (int i = 0; i < 4; ++i) {
        if (header[i] != expected[i])
            return false;
    }
    return true;
}

#endif // DESERIALISER_HXX