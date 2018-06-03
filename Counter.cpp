#include "Counter.hpp"

map<string, size_t> Counter::fill_map(vector<string>& vect) {
    map<string, size_t> part_map;
    for (auto it = vect.begin(); it < vect.end(); ++it) {
        if (*it == "") { break; }
        *it->erase(boost::remove_if(*it, boost::is_any_of(" ;?“!_(){}#[]//‘—`\"\',:.-\n\t")), it->end());
        boost::algorithm::to_lower(*it);
        ++part_map[*it];
    }
    return part_map;
}

void Counter::mapping_blocks(std::atomic<bool>& isActive, std::atomic<bool>& noBlockLeft, ThreadSafeDeque<vector<string>>& vector_deque, ThreadSafeDeque<map<string, size_t>>& map_deque) {
    
    std::unique_lock<recursive_mutex> u_lock(vector_deque.rec_mutex_);
    
    while (true) {
        if (vector_deque.empty() && !isActive) {
            noBlockLeft = true;
            map_deque.cond_var_.notify_all();
            return;
        }
        
        vector_deque.cond_var_.wait(u_lock, [&] { return !vector_deque.empty(); });
        
        if (!vector_deque.empty()) {
            auto block = vector_deque.pop_back();
            u_lock.unlock();
            map_deque.push_back(fill_map(block));
            map_deque.cond_var_.notify_one();
            u_lock.lock();
            continue;
        }
    }
}

void Counter::merging_blocks(std::atomic<bool>& noBlocksLeft, ThreadSafeDeque<map<string, size_t>>& map_deque) {
    
    std::unique_lock<recursive_mutex> u_lock(map_deque.rec_mutex_);
    
    while (true) {
        std::map<string, size_t> map;
        std::map<string, size_t> other_map;
        
        if (map_deque.size() == 1 && noBlocksLeft) {
            return;
        }
        
        map_deque.cond_var_.wait(u_lock, [&] { return (map_deque.size() > 1); });
        
        if (map_deque.size() > 1 ) {
            map = map_deque.pop_back();
            other_map = map_deque.pop_back();
            u_lock.unlock();
            
            for (auto& m : other_map) {
                map[m.first] += m.second;
            }
            
            other_map.clear();
            map_deque.push_front(std::move(map));
            u_lock.lock();
            continue;
        }
    }
}

vector<pair<string, size_t>> Counter::get_vector_pairs(const ThreadSafeDeque<map<std::string, size_t>>& safe_map) {
    auto words = get_map(safe_map);
    
    if (words.empty()) {
        cerr << "Error: map of words is empty, try func mapping_string() first" << endl;
        exit(-1);
    }
    
    vector<pair<string, size_t>> vector_pairs;
    
    for (auto& w : words) {
        vector_pairs.push_back(pair<string, size_t> (w.first, w.second));
    }
    return vector_pairs;
}

vector<pair<string, size_t>> Counter::get_vector_sorted_by_value(const ThreadSafeDeque<map<std::string, size_t>>& safe_map) {
    auto vector_pairs = get_vector_pairs(safe_map);
    
    std::sort(vector_pairs.begin(), vector_pairs.end(), [](auto &left, auto& right) {
        if (left.second < right.second) {
            return true;
        } else if (left.second == right.second) {
            if (left.first < right.first) {
                return true;
            }
        }
        return false;
    });
    
    return vector_pairs;
}

vector<pair<string, size_t>> Counter::get_vector_sorted_by_key(const ThreadSafeDeque<map<std::string, size_t>>& safe_map) {
    return get_vector_pairs(safe_map);
}

map<string, size_t> Counter::get_map(const ThreadSafeDeque<map<std::string, size_t>>& safe_map) {
    std::unique_lock<recursive_mutex> u_lock(safe_map.rec_mutex_);
    while (safe_map.size() > 1 && safe_map.size() == 0) {
        safe_map.cond_var_.wait(u_lock);
    }
    safe_map.cond_var_.notify_one();
    return safe_map.front();
}

void Counter::print_map_of_words(const ThreadSafeDeque<map<std::string, size_t>>& safe_map) {
    auto words = get_map(safe_map);
    if (words.empty()) {
        cerr << "Error: map of words is empty, try func mapping_string() first" << endl;
    }
    
    for (auto& m : words) {
        cout << m.first << " - " << m.second << endl;
    }
}
