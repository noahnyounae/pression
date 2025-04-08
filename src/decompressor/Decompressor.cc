#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

// Utility: read entire file into string
std::string readFile(const std::string &filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }
    std::ostringstream oss;
    oss << infile.rdbuf();
    return oss.str();
}

// Utility: split string by a delimiter into nonempty tokens
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            tokens.push_back(item);
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <compressed_file> <prs_file> <output_file>" << std::endl;
        return 1;
    }
    std::string compFile = argv[1];
    std::string prsFile = argv[2];
    std::string outFile = argv[3];
    
    // Read the compressed file content
    std::string compContent = readFile(compFile);
    if (compContent.empty()){
        std::cerr << "Failed to read compressed file content." << std::endl;
        return 1;
    }
    
    // Read the .prs file and split it by '#' to extract saved blocks.
    std::string prsContent = readFile(prsFile);
    if (prsContent.empty()){
        std::cerr << "Failed to read prs file content." << std::endl;
        return 1;
    }
    // Each block was written with an ending '#' (and blocks do not contain '#' themselves).
    std::vector<std::string> blocks = split(prsContent, '#');
    if (blocks.empty()){
        std::cerr << "No blocks found in prs file." << std::endl;
        return 1;
    }
    
    // Build mapping via regex: marker pattern "#(\\d*)#" where empty digits maps to block index 0.
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
        // Updated condition: cast index to size_t.
        if (static_cast<size_t>(index) < blocks.size())
            decompressed.append(blocks[index]);
        else
            decompressed.append(match.str());
        lastPos = match.position() + match.length();
        ++current;
    }
    decompressed.append(compContent.substr(lastPos));
    
    std::ofstream outfile(outFile, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open output file: " << outFile << std::endl;
        return 1;
    }
    outfile << decompressed;
    outfile.close();
    std::cout << "Decompression complete. Output written to " << outFile << std::endl;
    return 0;
}
