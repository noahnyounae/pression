#ifndef DECOMPRESSOR_HH
#define DECOMPRESSOR_HH

#include <string>

class Decompressor {
public:
    int run(const std::string &compressedFile, const std::string &prsFile, const std::string &outputFile);
};

#endif // DECOMPRESSOR_HH
