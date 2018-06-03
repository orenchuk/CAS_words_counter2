#ifndef Timer_hpp
#define Timer_hpp

#include <chrono>
#include <atomic>

typedef std::chrono::high_resolution_clock::time_point time_point_high_res;

class Timer {
private:
    time_point_high_res start_point;
    time_point_high_res stop_point;
    inline time_point_high_res get_current_time();
    template <class T> inline long long time_ms(const T&);
public:
    time_point_high_res start();
    time_point_high_res stop();
    long long total_time_ms();
};

#endif /* Timer_hpp */
