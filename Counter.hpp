#ifndef Counter_hpp
#define Counter_hpp

#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include "ThreadSafeDeque.hpp"
#include <utility>
#include <atomic>

using std::string;
using std::cerr;
using std::endl;
using std::cout;
using std::map;
using std::vector;
using std::thread;
using std::mutex;
using std::pair;

class Counter {
private:
    map<string, size_t> words;
    map<string, size_t> fill_map(vector<string>&);
    map<string, size_t> get_map(const ThreadSafeDeque<map<std::string, size_t> >&);
public:    
    void mapping_blocks(std::atomic<bool>&, std::atomic<bool>&, ThreadSafeDeque<vector<string>>&, ThreadSafeDeque<map<string, size_t>>&);
    void merging_blocks(std::atomic<bool>&, ThreadSafeDeque<map<string, size_t>>&);
    vector<pair<string, size_t>> get_vector_pairs(const ThreadSafeDeque<map<std::string, size_t>>&);
    vector<pair<string, size_t>> get_vector_sorted_by_key(const ThreadSafeDeque<map<std::string, size_t>>&);
    vector<pair<string, size_t>> get_vector_sorted_by_value(const ThreadSafeDeque<map<std::string, size_t>>&);
    void print_map_of_words(const ThreadSafeDeque<map<std::string, size_t>>&);
};

#endif /* Counter_hpp */
