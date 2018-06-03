#include "Timer.hpp"

inline time_point_high_res Timer::get_current_time() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template <class T>
inline long long Timer::time_ms(const T& t) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
}

time_point_high_res Timer::start() {
    start_point = get_current_time();
    return start_point;
}

time_point_high_res Timer::stop() {
    stop_point = get_current_time();
    return stop_point;
}

long long Timer::total_time_ms() {
    auto total_point = stop_point - start_point;
    return time_ms(total_point);
}
