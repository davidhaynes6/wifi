#pragma once
#include <ctime>

class Timer {
private:
    time_t startTime;
public:
    Timer() {
        startTime = time(nullptr);
    }

    bool hasElapsed(int duration) {
        return (time(nullptr) - startTime) >= duration;
    }

    void reset() {
        startTime = time(nullptr);
    }
};