#pragma once

#include <windows.h>

/**
 * @brief A high-resolution timer class using Windows performance counters.
 *
 * The Timer class provides precise timing functionality based on the Windows QueryPerformanceCounter API.
 * It is useful for benchmarking and measuring the execution time of operations.
 */
class Timer {
private:
    LARGE_INTEGER frequency;   ///< Frequency of the high-resolution performance counter (ticks per second).
    LARGE_INTEGER startTime;   ///< Time point when the timer was started.
    LARGE_INTEGER endTime;     ///< Time point when the timer was stopped.

public:
    /**
     * @brief Constructor. Initializes the frequency of the performance counter.
     */
    Timer();

    /**
     * @brief Destructor. No dynamic resources are allocated.
     */
    ~Timer() {}

    /**
     * @brief Starts the timer by recording the current time.
     */
    void start();

    /**
     * @brief Stops the timer by recording the current time.
     */
    void stop();

    /**
     * @brief Calculates the elapsed time in milliseconds between start and stop.
     *
     * @return Elapsed time in milliseconds.
     */
    double elapsedMilliseconds() const;

    /**
     * @brief Calculates the elapsed time in seconds between start and stop.
     *
     * @return Elapsed time in seconds.
     */
    double elapsedSeconds() const;

    /**
     * @brief Gets the raw number of ticks between start and stop.
     *
     * @return Time in ticks.
     */
    long long getTimeInTicks() const;

    /**
     * @brief Gets the frequency (ticks per second) of the high-resolution timer.
     *
     * @return Frequency in ticks per second.
     */
    long long getFrequency() const;

    /**
     * @brief Calculates elapsed time in milliseconds given a start tick and frequency.
     *
     * @param [in] start    Start time in ticks.
     * @param [in] freq     Frequency in ticks per second.
     * 
     * @return Elapsed time in milliseconds.
     */
    double elapsedTimeInMillis(long long start, long long freq) const;
};
