#ifndef COMPRESSOR_HXX
#define COMPRESSOR_HXX

#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <string>

// Helper: checkHeader
inline bool checkHeader(std::ifstream &infile, const char* expected) {
    char header[4];
    infile.read(header, 4);
    for (int i = 0; i < 4; ++i) {
        if (header[i] != expected[i])
            return false;
    }
    return true;
}

// getBinaryPayload
inline std::vector<char> getBinaryPayload(const std::string &filename, bool &success) {
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

// isBlockInPayload
inline bool isBlockInPayload(const std::vector<char>& payload, const std::vector<char>& block) {
    auto it = std::search(payload.begin(), payload.end(), block.begin(), block.end());
    return it != payload.end();
}

// findLargestIterationBlock
inline std::vector<char> findLargestIterationBlock(const std::vector<char>& payload, int length = 4) {
    if (payload.empty())
        return {};

    struct State {
        int len, link, cnt, firstPos;
        std::unordered_map<char, int> next;
    };

    int n = payload.size();
    std::vector<State> st(2 * n);
    int sz = 1, last = 0;
    st[0] = {0, -1, 0, 0, {}};

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

    int bestScore = 0, bestState = -1, bestLen = 0;
    for (int i = 0; i < sz; ++i) {
        if (st[i].len >= length && st[i].cnt > 1) {
            int score = st[i].len * st[i].cnt * st[i].cnt;
            if (score > bestScore) {
                int pos = st[i].firstPos;
                int start = pos - st[i].len + 1;
                bool containsHashtag = false;
                for (int j = start; j <= pos; ++j) {
                    if (payload[j] == '#') {
                        containsHashtag = true;
                        break;
                    }
                }
                if (!containsHashtag) {
                    bestScore = score;
                    bestState = i;
                    bestLen = st[i].len;
                }
            }
        }
    }
    if (bestState == -1 || bestLen < length)
        return {};

    int pos = st[bestState].firstPos;
    int start = pos - bestLen + 1;
    return std::vector<char>(payload.begin() + start, payload.begin() + pos + 1);
}

// replaceBlock
inline void replaceBlock(std::vector<char>& payload, const std::vector<char>& block, const std::string& replacement) {
    auto it = std::search(payload.begin(), payload.end(), block.begin(), block.end());
    while (it != payload.end()) {
        it = payload.erase(it, it + block.size());
        it = payload.insert(it, replacement.begin(), replacement.end());
        std::advance(it, replacement.size());
        it = std::search(it, payload.end(), block.begin(), block.end());
    }
}

// readBinaryFile
inline std::vector<char> readBinaryFile(const std::string &filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Erreur d'ouverture lors de la lecture de " << filename << std::endl;
        return {};
    }
    return std::vector<char>((std::istreambuf_iterator<char>(infile)),
                              std::istreambuf_iterator<char>());
}

// writeBinaryFile
inline bool writeBinaryFile(const std::string &filename, const std::vector<char> &data, bool append = false) {
    std::ofstream outfile(filename, append ? (std::ios::binary | std::ios::app) : std::ios::binary);
    if (!outfile) {
        std::cerr << "Erreur d'ouverture lors de l'écriture de " << filename << std::endl;
        return false;
    }
    outfile.write(data.data(), data.size());
    if (!outfile) {
        std::cerr << "Erreur lors de l'écriture des données dans " << filename << std::endl;
        return false;
    }
    return true;
}

// printIterationBlock
inline void printIterationBlock(const std::vector<char>& block) {
    std::cout << "Bloc d'itération trouvé : |";
    for (char c : block) {
        std::cout << c;
    }
    std::cout << "|" << std::endl;
}

#endif // COMPRESSOR_HXX