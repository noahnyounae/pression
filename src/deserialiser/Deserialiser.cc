#include "Deserialiser.hh"
#include "Deserialiser.hxx"

void Deserialiser::run(const std::string &filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Erreur d'ouverture du fichier binaire." << std::endl;
        return;
    }

    // Vérification du header
    const char expectedHeader[4] = {'P', 'R', 'S', 'N'};
    if (!checkHeader(infile, expectedHeader)) {
        std::cerr << "Format de fichier inconnu." << std::endl;
        return;
    }

    // Lecture de la version
    uint8_t version;
    infile.read(reinterpret_cast<char*>(&version), sizeof(version));

    // Lecture de l'enregistrement
    uint8_t type;
    infile.read(reinterpret_cast<char*>(&type), sizeof(type));
    uint32_t length;
    infile.read(reinterpret_cast<char*>(&length), sizeof(length));

    std::vector<char> data(length);
    infile.read(data.data(), length);
    infile.close();

    // Si le type est 1, il s'agit d'un contenu texte
    if (type == 1) {
        std::string text(data.begin(), data.end());
        std::ofstream outfile("output.txt");
        if (!outfile) {
            std::cerr << "Erreur d'ouverture du fichier de sortie." << std::endl;
            return;
        }
        outfile << text;
        outfile.close();
        std::cout << "Le fichier texte a été reconstruit avec succès." << std::endl;
    } else {
        std::cerr << "Type de données non supporté pour l'extraction en texte." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    Deserialiser d;
    d.run(inputFile);
    return 0;
}