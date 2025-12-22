#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iomanip>

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

class Searcher {
private:
    struct IndexEntry {
        char* word;
        int* docs;
        int count;
    };

    IndexEntry* entries;
    int entries_count;
    int capacity;
    Stemmer stemmer;

    void grow() {
        capacity *= 2;
        IndexEntry* new_entries = new IndexEntry[capacity];
        memcpy(new_entries, entries, sizeof(IndexEntry) * entries_count);
        delete[] entries;
        entries = new_entries;
    }

    void to_lowercase(char* s) {
        for (int i = 0; s[i]; i++) {
            unsigned char c1 = s[i];
            unsigned char c2 = s[i+1];
            if (c1 == 0xD0 && c2 >= 0x90 && c2 <= 0xAF) {
                s[i+1] += 0x20;
                i++;
            } else if (c1 >= 'A' && c1 <= 'Z') {
                s[i] = c1 + ('a' - 'A');
            }
        }
    }

public:
    Searcher() : entries_count(0), capacity(10000) {
        entries = new IndexEntry[capacity];
    }

    void load_index(const char* filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
            std::cerr << "Error opening index file: " << filename << std::endl;
            return;
        }

        while (in.peek() != EOF) {
            if (entries_count == capacity) grow();

            int word_len;
            in.read((char*)&word_len, sizeof(int));
            if (in.eof()) break;

            entries[entries_count].word = new char[word_len + 1];
            in.read(entries[entries_count].word, word_len);
            entries[entries_count].word[word_len] = '\0';

            in.read((char*)&entries[entries_count].count, sizeof(int));
            entries[entries_count].docs = new int[entries[entries_count].count];
            in.read((char*)entries[entries_count].docs, sizeof(int) * entries[entries_count].count);
            
            std::sort(entries[entries_count].docs, entries[entries_count].docs + entries[entries_count].count);

            entries_count++;
        }
        in.close();
        std::cout << "Loaded " << entries_count << " words from index." << std::endl;
    }

    IndexEntry* find_word(const char* word) {
        char buf[1024];
        strncpy(buf, word, 1023);
        buf[1023] = '\0';
        to_lowercase(buf);
        stemmer.stem(buf);

        for (int i = 0; i < entries_count; i++) {
            if (strcmp(entries[i].word, buf) == 0) {
                return &entries[i];
            }
        }
        return nullptr;
    }

    void search_and(const char* word1, const char* word2) {
        IndexEntry* e1 = find_word(word1);
        IndexEntry* e2 = find_word(word2);

        if (!e1 || !e2) {
            std::cout << "No results found for: " << word1 << " AND " << word2 << std::endl;
            return;
        }

        std::cout << "Results for: " << word1 << " AND " << word2 << std::endl;
        
        int i = 0, j = 0;
        int found = 0;
        while (i < e1->count && j < e2->count) {
            if (e1->docs[i] == e2->docs[j]) {
                printf("doc_%05d.txt\n", e1->docs[i]);
                i++; j++;
                found++;
            } else if (e1->docs[i] < e2->docs[j]) {
                i++;
            } else {
                j++;
            }
        }
        std::cout << "Total found: " << found << std::endl;
    }

    void search_or(const char* word1, const char* word2) {
        IndexEntry* e1 = find_word(word1);
        IndexEntry* e2 = find_word(word2);

        if (!e1 && !e2) {
            std::cout << "No results found." << std::endl;
            return;
        }

        std::cout << "Results for: " << word1 << " OR " << word2 << std::endl;
        
        int i = 0, j = 0;
        int found = 0;
        while (i < (e1 ? e1->count : 0) || j < (e2 ? e2->count : 0)) {
            int id;
            if (e1 && i < e1->count && e2 && j < e2->count) {
                if (e1->docs[i] == e2->docs[j]) {
                    id = e1->docs[i];
                    i++; j++;
                } else if (e1->docs[i] < e2->docs[j]) {
                    id = e1->docs[i];
                    i++;
                } else {
                    id = e2->docs[j];
                    j++;
                }
            } else if (e1 && i < e1->count) {
                id = e1->docs[i++];
            } else {
                id = e2->docs[j++];
            }
            printf("doc_%05d.txt\n", id);
            found++;
            if (found > 50) { 
                std::cout << "... (truncated)" << std::endl;
                break;
            }
        }
        std::cout << "Total found: " << found << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <index_file>" << std::endl;
        return 1;
    }

    Searcher searcher;
    searcher.load_index(argv[1]);

    char cmd[1024], w1[1024], w2[1024];
    std::cout << "Commands: AND word1 word2, OR word1 word2, EXIT" << std::endl;
    
    while (true) {
        std::cout << "> ";
        if (!(std::cin >> cmd)) break;
        
        if (strcasecmp(cmd, "EXIT") == 0) break;
        
        if (strcasecmp(cmd, "AND") == 0) {
            if (std::cin >> w1 >> w2) searcher.search_and(w1, w2);
        } else if (strcasecmp(cmd, "OR") == 0) {
            if (std::cin >> w1 >> w2) searcher.search_or(w1, w2);
        } else {
            std::cout << "Unknown command. Use AND, OR or EXIT." << std::endl;
        }
    }

    return 0;
}
