#include "Decompressor.hh"
#include "Decompressor.hxx"
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

int Decompressor::run(const std::string &compressedFile, const std::string &prsFile, const std::string &outputFile) {
    std::string compContent = readFile(compressedFile);
    if (compContent.empty()){
        std::cerr << "Failed to read compressed file content." << std::endl;
        return 1;
    }
    std::string prsContent = readFile(prsFile);
    if (prsContent.empty()){
        std::cerr << "Failed to read prs file content." << std::endl;
        return 1;
    }
    std::vector<std::string> blocks = split(prsContent, '#');
    if (blocks.empty()){
        std::cerr << "No blocks found in prs file." << std::endl;
        return 1;
    }
    std::regex markerRegex("#(\\d*)#");
    std::string decompressed;
    std::sregex_iterator current(compContent.begin(), compContent.end(), markerRegex);
    std::sregex_iterator end;
    size_t lastPos = 0;
    while (current != end) {
        std::smatch match = *current;
        decompressed.append(compContent.substr(lastPos, match.position() - lastPos));
        int index = 0;
        std::string numStr = match[1].str();
        if (!numStr.empty())
            index = std::stoi(numStr);
        if (static_cast<size_t>(index) < blocks.size())
            decompressed.append(blocks[index]);
        else
            decompressed.append(match.str());
        lastPos = match.position() + match.length();
        ++current;
    }
    decompressed.append(compContent.substr(lastPos));
    
    std::ofstream outfile(outputFile, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return 1;
    }
    outfile << decompressed;
    outfile.close();
    std::cout << "Decompression complete. Output written to " << outputFile << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <compressed_file>" << std::endl;
        return 1;
    }
    std::string compFile = argv[1];
    std::string cfgFile = compFile + ".cfg";
    std::string outFile;
    size_t pos = compFile.find_last_of('.');
    if (pos != std::string::npos)
        outFile = compFile.substr(0, pos);
    else
        outFile = compFile;
    outFile += ".dprs";
    Decompressor decompressor;
    return decompressor.run(compFile, cfgFile, outFile);
}
