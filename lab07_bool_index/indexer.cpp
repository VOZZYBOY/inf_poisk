#include <iostream>
#include <fstream>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

const int HASH_SIZE = 1000003;

struct Posting {
    int doc_id;
    Posting* next;
};

struct WordEntry {
    char* word;
    Posting* postings;
    WordEntry* next;
};

class Indexer {
private:
    WordEntry** table;

    unsigned int hash(const char* str) {
        unsigned int h = 0;
        while (*str) {
            h = h * 31 + (unsigned char)*str++;
        }
        return h % HASH_SIZE;
    }

public:
    Indexer() {
        table = new WordEntry*[HASH_SIZE];
        memset(table, 0, sizeof(WordEntry*) * HASH_SIZE);
    }

    void add_word(const char* word, int doc_id) {
        unsigned int h = hash(word);
        WordEntry* entry = table[h];
        
        while (entry) {
            if (strcmp(entry->word, word) == 0) {
                if (entry->postings && entry->postings->doc_id == doc_id) {
                    return;
                }
                Posting* p = new Posting;
                p->doc_id = doc_id;
                p->next = entry->postings;
                entry->postings = p;
                return;
            }
            entry = entry->next;
        }

        WordEntry* new_entry = new WordEntry;
        new_entry->word = new char[strlen(word) + 1];
        strcpy(new_entry->word, word);
        
        Posting* p = new Posting;
        p->doc_id = doc_id;
        p->next = nullptr;
        
        new_entry->postings = p;
        new_entry->next = table[h];
        table[h] = new_entry;
    }

    void save_index(const char* filename) {
        std::ofstream out(filename, std::ios::binary);
        if (!out.is_open()) return;

        for (int i = 0; i < HASH_SIZE; i++) {
            WordEntry* entry = table[i];
            while (entry) {
                int word_len = strlen(entry->word);
                out.write((char*)&word_len, sizeof(int));
                out.write(entry->word, word_len);

                int count = 0;
                Posting* p = entry->postings;
                while (p) {
                    count++;
                    p = p->next;
                }
                out.write((char*)&count, sizeof(int));

                p = entry->postings;
                while (p) {
                    out.write((char*)&p->doc_id, sizeof(int));
                    p = p->next;
                }
                entry = entry->next;
            }
        }
        out.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <stems_dir> <index_file>" << std::endl;
        return 1;
    }

    Indexer indexer;
    DIR* dir = opendir(argv[1]);
    if (!dir) return 1;

    struct dirent* ent;
    int processed = 0;
    while ((ent = readdir(dir)) != nullptr) {
        if (ent->d_name[0] == '.') continue;

        int doc_id = atoi(ent->d_name + 4);
        
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", argv[1], ent->d_name);
        
        std::ifstream in(path);
        char word[1024];
        while (in >> word) {
            indexer.add_word(word, doc_id);
        }
        
        processed++;
        if (processed % 1000 == 0) {
            std::cout << "Indexed " << processed << " files..." << std::endl;
        }
    }
    closedir(dir);

    std::cout << "Saving index..." << std::endl;
    indexer.save_index(argv[2]);
    std::cout << "Done!" << std::endl;

    return 0;
}
