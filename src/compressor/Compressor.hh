#ifndef COMPRESSOR_HH
#define COMPRESSOR_HH

#include <string>

class Compressor {
public:
    int run(const std::string &inputFile, const std::string &outputFile, int blockSize = 4);
};

#endif // COMPRESSOR_HH
