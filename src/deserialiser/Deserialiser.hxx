#ifndef DESERIALISER_HXX
#define DESERIALISER_HXX

#include "Deserialiser.hh"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <algorithm>

inline bool checkHeader(std::ifstream &infile, const char* expected) {
    char header[4];
    infile.read(header, 4);
    for (int i = 0; i < 4; ++i) {
        if (header[i] != expected[i])
            return false;
    }
    return true;
}

inline std::vector<char> getBinaryPayload(const char* filename, bool &success) {
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

    // Lecture de la version (1 octet), du type (1 octet) et de la longueur (4 octets)
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

inline std::vector<char> findLargestIterationBlock(const std::vector<char>& payload) {
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
                st[clone] = {st[p].len + 1, st[q].link, 0, st[q].firstPos, st[q].next}; // clone transitions
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

#endif // DESERIALISER_HXX