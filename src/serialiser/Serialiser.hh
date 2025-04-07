#ifndef SERIALISER_HH
#define SERIALISER_HH

#include <cstdint>
#include <vector>
#include <string>

struct BinaryRecord {
    uint8_t type;      // 1 pour du texte
    uint32_t length;   // longueur en octets
    std::vector<char> data; // données
};

class Serialiser {
public:
    void run(const std::string &inputFile);
};

#endif // SERIALISER_HH