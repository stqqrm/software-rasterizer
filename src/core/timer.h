#pragma once
#include <chrono>

namespace core {

class timer {
    using clock = std::chrono::high_resolution_clock;
    using seconds = std::chrono::duration<double>;
public:
    timer();
    double elapsed();
    double restart();
    void   start();
    double stop();
private:
    clock::time_point _start;
};

} // namespace core
