#ifndef DESERIALISER_HH
#define DESERIALISER_HH

#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>

class Deserialiser {
public:
    void run(const std::string &filename);
};

#endif // DESERIALISER_HH