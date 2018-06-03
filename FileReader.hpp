#ifndef FileReader_hpp
#define FileReader_hpp

#include <string>
#include <iostream>
#include <vector>
#include "ThreadSafeDeque.hpp"
#include <fstream>
#include <atomic>

class FileReader {
public:
    template <class T> static void read_file(const std::string&, const size_t&, T&, std::atomic<bool>&);
    template <class T> static void write_file(const T&, const string&);
};

template <class T>
void FileReader::read_file(const std::string& filename, const size_t& block_size, T& deque_words, std::atomic<bool>& isActive) {
    cout << "block_size: " << block_size << endl;
    std::ifstream input(filename);
    string word;
    size_t words_index = 0;
    std::vector<string> words;
    
    while (input >> word) {
        words.push_back(std::move(word));
        ++words_index;
        if (words_index >= block_size) {
            deque_words.emplace_back(std::move(words));
            words_index = 0;
            words.clear();
        }
    }
    if(words.size() > 0)
        deque_words.emplace_back(std::move(words));
    
    isActive = false;
    input.close();
}

template <class T>
void FileReader::write_file(const T& vect, const string& filename) {
    std::ofstream outfile(filename);
    int index = 0;
    
    for (auto& v : vect) {
        ++index;
        outfile << index << ". " << v.first << "\t" << v.second << std::endl;
    }
    
    outfile.close();
}

#endif /* FileReader_hpp */
