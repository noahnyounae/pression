#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <string>
#include "Compressor.hh"
#include "Compressor.hxx"

int Compressor::run(const std::string &inputFile, const std::string &outputFile, int blockSize) {
    const std::string cfgFile = outputFile + ".cfg";
    bool success;
    std::vector<char> payload = getBinaryPayload(inputFile, success);
    if (!success) {
        std::cerr << "Erreur lors de la récupération du contenu binaire." << std::endl;
        return 1;
    }
    std::vector<char> largestBlock = findLargestIterationBlock(payload, blockSize);
    while (!largestBlock.empty()) {
        int balise = 0;
        std::string replacement = "#" + std::to_string(balise) + "#";
        while (isBlockInPayload(payload, std::vector<char>(replacement.begin(), replacement.end()))) {
            balise++;
            replacement = "#" + std::to_string(balise) + "#";
        }
        replaceBlock(payload, largestBlock, replacement);
        //printIterationBlock(largestBlock);
        largestBlock.push_back('#');
        writeBinaryFile(cfgFile, largestBlock, true);
        largestBlock = findLargestIterationBlock(payload, blockSize);
    }
    if (!writeBinaryFile(outputFile, payload)) {
        std::cerr << "Erreur lors de l'écriture du fichier binaire." << std::endl;
        return 1;
    }
    std::cout << "Le bloc d'itération le plus important a été écrit dans " << outputFile << std::endl;
    return 0;
}

// Main function remains unchanged.
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    std::string outFile;
    size_t pos = inputFile.find_last_of('.');
    if (pos != std::string::npos)
        outFile = inputFile.substr(0, pos);
    else
        outFile = inputFile;
    outFile += ".prs";
    Compressor comp;
    return comp.run(inputFile, outFile, 6);
}