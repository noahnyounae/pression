#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <string>

// Helper: checkHeader
static bool checkHeader(std::ifstream &infile, const char* expected) {
    char header[4];
    infile.read(header, 4);
    for (int i = 0; i < 4; ++i) {
        if (header[i] != expected[i])
            return false;
    }
    return true;
}

// Moved from Deserialiser.hxx
std::vector<char> getBinaryPayload(const std::string &filename, bool &success) {
    std::vector<char> payload;
    success = false;
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Erreur d'ouverture du fichier binaire." << std::endl;
        return payload;
    }
    const char expectedHeader[4] = {'P', 'R', 'S', 'N'};
    if (!checkHeader(infile, expectedHeader)) {
        std::cerr << "Format de fichier inconnu." << std::endl;
        return payload;
    }
    uint8_t version;
    infile.read(reinterpret_cast<char*>(&version), sizeof(version));
    uint8_t recordType;
    infile.read(reinterpret_cast<char*>(&recordType), sizeof(recordType));
    uint32_t length;
    infile.read(reinterpret_cast<char*>(&length), sizeof(length));
    if (length == 0) {
        std::cerr << "Aucune donnée binaire trouvée." << std::endl;
        return payload;
    }
    payload.resize(length);
    infile.read(payload.data(), length);
    if (!infile) {
        std::cerr << "Erreur lors de la lecture des données binaires." << std::endl;
        return payload;
    }
    success = true;
    return payload;
}

// Moved from Deserialiser.hxx
std::vector<char> findLargestIterationBlock(const std::vector<char>& payload) {
    if (payload.empty())
        return {};

    // Suffix automaton state
    struct State {
        int len, link, cnt, firstPos;
        std::unordered_map<char, int> next;
    };

    int n = payload.size();
    std::vector<State> st(2 * n);
    int sz = 1, last = 0;
    st[0] = {0, -1, 0, 0, {}};

    // Build automaton
    for (int i = 0; i < n; ++i) {
        char c = payload[i];
        int cur = sz++;
        st[cur] = {st[last].len + 1, 0, 1, i, {}};
        int p = last;
        for (; p != -1 && !st[p].next.count(c); p = st[p].link)
            st[p].next[c] = cur;
        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = sz++;
                st[clone] = {st[p].len + 1, st[q].link, 0, st[q].firstPos, st[q].next};
                for (; p != -1 && st[p].next[c] == q; p = st[p].link)
                    st[p].next[c] = clone;
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    // Propagate counts in order of decreasing length
    std::vector<int> order(sz);
    for (int i = 0; i < sz; ++i)
        order[i] = i;
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return st[a].len > st[b].len;
    });
    for (int i : order) {
        if (st[i].link != -1)
            st[st[i].link].cnt += st[i].cnt;
    }

    // Find the state with the best score (only for substrings of length >= 4)
    int bestScore = 0, bestState = -1, bestLen = 0;
    for (int i = 0; i < sz; ++i) {
        if (st[i].len >= 4 && st[i].cnt > 1) {
            int score = st[i].len * st[i].cnt * st[i].cnt;
            if (score > bestScore) {
                bestScore = score;
                bestState = i;
                bestLen = st[i].len;
            }
        }
    }
    if (bestState == -1)
        return {};

    int pos = st[bestState].firstPos;
    int start = pos - bestLen + 1;
    return std::vector<char>(payload.begin() + start, payload.begin() + pos + 1);
}

// New method: Read the entire .bin file into a char vector
std::vector<char> readBinaryFile(const std::string &filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Erreur d'ouverture lors de la lecture de " << filename << std::endl;
        return {};
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(infile)),
                              std::istreambuf_iterator<char>());
    return buffer;
}

// New method: Write a char vector to a .bin file
bool writeBinaryFile(const std::string &filename, const std::vector<char> &data) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Erreur d'ouverture lors de l'écriture de " << filename << std::endl;
        return false;
    }
    outfile.write(data.data(), data.size());
    return outfile.good();
}

// Main function to demonstrate the functionality
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    // Read the binary file
    bool success;
    std::vector<char> payload = getBinaryPayload(inputFile, success);
    if (!success) {
        std::cerr << "Erreur lors de la récupération du contenu binaire." << std::endl;
        return 1;
    }

    // Find the largest iteration block
    std::vector<char> largestBlock = findLargestIterationBlock(payload);
    if (largestBlock.empty()) {
        std::cerr << "Aucun bloc d'itération trouvé." << std::endl;
        return 1;
    }

    // Write the largest block to the std out
    if (!writeBinaryFile(outputFile, largestBlock)) {
        std::cerr << "Erreur lors de l'écriture du fichier binaire." << std::endl;
        return 1;
    }
    std::cout << "Le bloc d'itération le plus important a été écrit dans " << outputFile << std::endl;
    std::cout << "Bloc d'itération le plus important (taille * récurrence): |";
    for (const auto &ch : largestBlock) {
        std::cout << ch;
    }
    std::cout << "|" << std::endl;
    return 0;
}