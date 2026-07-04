#include "timer.h"

namespace core {

timer::timer() : _start(clock::now()) {}

double timer::elapsed() {
    return std::chrono::duration_cast<seconds>(clock::now() - _start).count();
}

double timer::restart() {
    auto   now = clock::now();
    double dt = std::chrono::duration_cast<seconds>(now - _start).count();
    _start = now;
    return dt;
}

void timer::start() {
    _start = clock::now();
}

double timer::stop() {
    return std::chrono::duration_cast<seconds>(clock::now() - _start).count();
}

} // namespace core
