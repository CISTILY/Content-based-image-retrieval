#include "Time.h"

Timer::Timer() {
    QueryPerformanceFrequency(&frequency);
}

void Timer::start() {
    QueryPerformanceCounter(&startTime);
}

void Timer::stop() {
    QueryPerformanceCounter(&endTime);
}

double Timer::elapsedMilliseconds() const {
    return static_cast<double>(endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
}

double Timer::elapsedSeconds() const {
    return static_cast<double>(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
}

long long Timer::getTimeInTicks() const {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now.QuadPart;
}

long long Timer::getFrequency() const {
    return frequency.QuadPart;
}

double Timer::elapsedTimeInMillis(long long start, long long freq) const {
    long long now = getTimeInTicks();
    return static_cast<double>(now - start) * 1000.0 / freq;
}