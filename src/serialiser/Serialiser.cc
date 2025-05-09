#include "Serialiser.hh"
#include "Serialiser.hxx"
#include <fstream>
#include <iterator>
#include <iostream>

void Serialiser::run(const std::string &inputFile, const std::string &outputFile) {
    // Lecture du contenu du fichier texte
    std::ifstream infile(inputFile, std::ios::binary);
    if (!infile) {
        std::cerr << "Erreur d'ouverture du fichier texte." << std::endl;
        return;
    }
    std::string text((std::istreambuf_iterator<char>(infile)),
                      std::istreambuf_iterator<char>());
    infile.close();

    // Préparation de l'enregistrement pour du texte (type 1)
    BinaryRecord record;
    record.type = 1; // type texte
    record.length = text.size();
    record.data.assign(text.begin(), text.end());

    if (writeRecordToFile(record, outputFile.c_str())) {
        std::cout << "Transformation du fichier texte en format binaire réussie." << std::endl;
    }
}

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
    outFile += ".bin";
    Serialiser s;
    s.run(inputFile, outFile);
    return 0;
}