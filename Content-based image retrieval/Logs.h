#pragma once

#include <vector>
#include <string>

using namespace std;

/**
 * @class Log
 * @brief A lightweight logging utility for recording image and feature database processing status.
 *
 * This class provides separate logs for image-related and feature-related messages, allowing
 * structured reporting and debugging during image database operations and feature extraction.
 */
class Log {
private:
    vector<string> imageDatabaseLog;     ///< Stores log messages related to image database operations.
    vector<string> featureDatabaseLog;   ///< Stores log messages related to feature extraction or indexing.

public:
    /**
     * @brief Default constructor.
     */
    Log() {}

    /**
     * @brief Default destructor.
     */
    ~Log() {}

    /**
     * @brief Writes a log message to the image database log.
     *
     * @param[in] msg  A string message describing an event or status related to image loading.
     *
     * @return void
     */
    void writeToImageDatabaseLog(string msg);

    /**
     * @brief Writes a log message to the feature database log.
     *
     * @param[in] msg  A string message describing an event or status related to feature extraction.
     *
     * @return void
     */
    void writeToFeatureDatabaseLog(string msg);

    /**
     * @brief Retrieves the image database log entries.
     *
     * @return A vector of strings containing image-related log messages.
     */
    vector<string> getImageDatabaseLog();

    /**
     * @brief Retrieves the feature database log entries.
     *
     * @return A vector of strings containing feature-related log messages.
     */
    vector<string> getFeatureDatabaseLog();
};
