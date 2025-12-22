#include <iostream>
#include <fstream>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    std::ifstream in(argv[1], std::ios::binary);
    for (int i = 0; i < 100; i++) {
        int len;
        in.read((char*)&len, sizeof(int));
        if (in.eof()) break;
        char* word = new char[len + 1];
        in.read(word, len);
        word[len] = '\0';
        int count;
        in.read((char*)&count, sizeof(int));
        std::cout << word << " (" << count << ")" << std::endl;
        in.seekg(count * sizeof(int), std::ios::cur);
        delete[] word;
    }
    return 0;
}
