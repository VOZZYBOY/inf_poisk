#include <iostream>
#include <fstream>
#include <cstring>

class Stemmer {
public:
    void stem(char* word) {
        int len = strlen(word);
        if (len < 4) return;

        int rv = find_rv(word, len);
        if (rv == -1) return;

        if (!replace(word, rv, "ившись", "") && !replace(word, rv, "ывшись", "")) {
            replace(word, rv, "ившись", "");
        }

        replace(word, rv, "ся", "");
        replace(word, rv, "сь", "");

        if (replace(word, rv, "ому", "") || replace(word, rv, "ему", "") || 
            replace(word, rv, "ими", "") || replace(word, rv, "ыми", "") ||
            replace(word, rv, "ого", "") || replace(word, rv, "его", "") ||
            replace(word, rv, "ой", "") || replace(word, rv, "ей", "") ||
            replace(word, rv, "ая", "") || replace(word, rv, "яя", "") ||
            replace(word, rv, "ое", "") || replace(word, rv, "ее", "") ||
            replace(word, rv, "ых", "") || replace(word, rv, "их", "")) {
        } else {
            if (!replace(word, rv, "ешь", "") && !replace(word, rv, "ишь", "") &&
                !replace(word, rv, "ете", "") && !replace(word, rv, "ите", "") &&
                !replace(word, rv, "ют", "") && !replace(word, rv, "ят", "") &&
                !replace(word, rv, "ла", "") && !replace(word, rv, "ло", "") &&
                !replace(word, rv, "ли", "")) {
                
                replace(word, rv, "ами", "");
                replace(word, rv, "ями", "");
                replace(word, rv, "иям", "");
                replace(word, rv, "иях", "");
                replace(word, rv, "ом", "");
                replace(word, rv, "ем", "");
                replace(word, rv, "ам", "");
                replace(word, rv, "ям", "");
                replace(word, rv, "ов", "");
                replace(word, rv, "ев", "");
                replace(word, rv, "а", "");
                replace(word, rv, "е", "");
                replace(word, rv, "и", "");
                replace(word, rv, "о", "");
                replace(word, rv, "у", "");
                replace(word, rv, "ы", "");
                replace(word, rv, "я", "");
                replace(word, rv, "ь", "");
                replace(word, rv, "й", "");
            }
        }
    }

private:
    bool is_vowel(unsigned char c1, unsigned char c2) {
        if (c1 == 0xD0) {
            return (c2 == 0xB0 || c2 == 0xB5 || c2 == 0xB8 || c2 == 0xBE || c2 == 0xBA || c2 == 0xBB);
        }
        if (c1 == 0xD1) {
            return (c2 == 0x83 || c2 == 0x8B || c2 == 0x8D || c2 == 0x8E || c2 == 0x8F || c2 == 0x91);
        }
        return false;
    }

    int find_rv(char* word, int len) {
        for (int i = 0; i < len - 1; i++) {
            if (is_vowel((unsigned char)word[i], (unsigned char)word[i+1])) {
                return i + 2;
            }
        }
        return -1;
    }

    bool replace(char* word, int rv, const char* suffix, const char* replacement) {
        int word_len = strlen(word);
        int suffix_len = strlen(suffix);
        if (word_len - rv < suffix_len) return false;

        if (strcmp(word + word_len - suffix_len, suffix) == 0) {
            word[word_len - suffix_len] = '\0';
            strcat(word, replacement);
            return true;
        }
        return false;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_tokens> <output_stems>" << std::endl;
        return 1;
    }

    std::ifstream inFile(argv[1]);
    std::ofstream outFile(argv[2]);

    if (!inFile.is_open() || !outFile.is_open()) return 1;

    Stemmer stemmer;
    char buffer[1024];
    while (inFile >> buffer) {
        stemmer.stem(buffer);
        if (buffer[0] != '\0') {
            outFile << buffer << "\n";
        }
    }

    return 0;
}
