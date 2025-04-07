#ifndef SERIALISER_HXX
#define SERIALISER_HXX

#include "Serialiser.hh"
#include <fstream>
#include <iostream>

inline bool writeRecordToFile(const BinaryRecord& record, const char* filename) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Erreur d'ouverture du fichier binaire." << std::endl;
        return false;
    }
    // Écriture du header
    const char header[4] = {'P', 'R', 'S', 'N'};
    outfile.write(header, 4);
    // Écriture de la version (par exemple, 1)
    uint8_t version = 1;
    outfile.write(reinterpret_cast<const char*>(&version), sizeof(version));
    // Écriture de l'enregistrement
    outfile.write(reinterpret_cast<const char*>(&record.type), sizeof(record.type));
    outfile.write(reinterpret_cast<const char*>(&record.length), sizeof(record.length));
    outfile.write(record.data.data(), record.data.size());
    return true;
}

#endif // SERIALISER_HXX