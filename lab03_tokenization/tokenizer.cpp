#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class MyTokenizer {
public:
    void processFile(const std::string& inputPath, const std::string& outputPath) {
        std::ifstream inFile(inputPath);
        std::ofstream outFile(outputPath);

        if (!inFile.is_open() || !outFile.is_open()) {
            std::cerr << "Error opening files: " << inputPath << std::endl;
            return;
        }

        std::string word;
        while (inFile >> word) {
            std::string cleanWord = cleanAndLower(word);
            if (!cleanWord.empty()) {
                outFile << cleanWord << "\n";
            }
        }
    }

private:
    std::string cleanAndLower(const std::string& word) {
        std::string result;
        for (size_t i = 0; i < word.length(); ++i) {
            unsigned char c = static_cast<unsigned char>(word[i]);

            if (std::isalnum(c)) {
                result += static_cast<char>(std::tolower(c));
            }
            else if (c == 0xD0 || c == 0xD1) {
                if (i + 1 < word.length()) {
                    unsigned char next = static_cast<unsigned char>(word[i + 1]);
                    
                    if (c == 0xD0 && next >= 0x90 && next <= 0x9F) {
                        result += static_cast<char>(0xD0);
                        result += static_cast<char>(next + 0x20);
                    } else if (c == 0xD0 && next >= 0xA0 && next <= 0xAF) {
                        result += static_cast<char>(0xD1);
                        result += static_cast<char>(next - 0x20);
                    } else if (c == 0xD0 && next == 0x81) {
                        result += static_cast<char>(0xD1);
                        result += static_cast<char>(0x91);
                    } else {
                        result += static_cast<char>(c);
                        result += static_cast<char>(next);
                    }
                    i++; 
                }
            }
        }
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    MyTokenizer tokenizer;
    tokenizer.processFile(argv[1], argv[2]);

    return 0;
}
