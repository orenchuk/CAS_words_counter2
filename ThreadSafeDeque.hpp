#ifndef ThreadSafeDeque_hpp
#define ThreadSafeDeque_hpp

#include <deque>
#include <string>
#include <mutex>
#include <condition_variable>

using std::deque;
using std::string;
using std::recursive_mutex;
using std::mutex;
using std::condition_variable_any;

template<class T>
class ThreadSafeDeque {
    deque<T> thread_safe_deque_{};
    
public:
    mutable recursive_mutex rec_mutex_{};
    mutable mutex mutex_{};
    mutable condition_variable_any cond_var_{};
    size_t size() const;
    bool empty() const;
    T pop_front();
    T pop_back();
    void push_front(const T& value);
    void push_back(const T& value);
    void emplace_front(const T&& value);
    void emplace_back(const T&& value);
    const T& front() const;
    const T& back() const;
};

template<class T>
size_t ThreadSafeDeque<T>::size() const {
    std::lock_guard<recursive_mutex> lock(rec_mutex_);
    return thread_safe_deque_.size();
}

template<class T>
bool ThreadSafeDeque<T>::empty() const {
    std::lock_guard<recursive_mutex> lock(rec_mutex_);
    return thread_safe_deque_.empty();
}

template<class T>
const T& ThreadSafeDeque<T>::front() const {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    while (thread_safe_deque_.empty()) {
        cond_var_.wait(u_lock);
    }
    return thread_safe_deque_.front();
}

template<class T>
const T& ThreadSafeDeque<T>::back() const {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    while (thread_safe_deque_.empty()) {
        cond_var_.wait(u_lock);
    }
    return thread_safe_deque_.back();
}

template<class T>
T ThreadSafeDeque<T>::pop_front() {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    while (thread_safe_deque_.empty()) {
        cond_var_.wait(u_lock);
    }
    auto front_value = thread_safe_deque_.front();
    thread_safe_deque_.pop_front();
    return front_value;
}

template<class T>
T ThreadSafeDeque<T>::pop_back() {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    while (thread_safe_deque_.empty()) {
        cond_var_.wait(u_lock);
    }
    auto back_value = thread_safe_deque_.back();
    thread_safe_deque_.pop_back();
    return back_value;
}

template <class T>
void ThreadSafeDeque<T>::push_front(const T& value) {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    thread_safe_deque_.push_front(value);
    u_lock.unlock();
    cond_var_.notify_one();
}

template <class T>
void ThreadSafeDeque<T>::push_back(const T& value) {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    thread_safe_deque_.push_back(value);
    u_lock.unlock();
    cond_var_.notify_one();
}

template <class T>
void ThreadSafeDeque<T>::emplace_back(const T&& value) {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    thread_safe_deque_.emplace_back(value);
    u_lock.unlock();
    cond_var_.notify_one();
}

template <class T>
void ThreadSafeDeque<T>::emplace_front(const T&& value) {
    std::unique_lock<recursive_mutex> u_lock(rec_mutex_);
    thread_safe_deque_.emplace_front(value);
    u_lock.unlock();
    cond_var_.notify_one();
}


#endif /* ThreadSafeDeque_hpp */
