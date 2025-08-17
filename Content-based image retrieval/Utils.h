#pragma once

#include <string>
#include <algorithm>
#include <shlobj.h>

using namespace std;

/**
 * @class Utils
 * @brief Provides utility functions for path and string manipulation.
 *
 * This class includes helper functions commonly used in file handling,
 * such as extracting filenames, feature types, or paths from full strings.
 */
class Utils {
public:
    /**
     * @brief Extracts the file name (without extension) from a given path.
     * 
     * @param[in] fullPath The full file path string.
     * 
     * @return A string containing the file name without extension.
     *
     * @note Example: "C:/data/image.jpg" → "image"
     */
    string extractFileName(string fullPath);

    /**
     * @brief Extracts the feature name from a full path or file name.
     * 
     * @param[in] path The input string possibly containing the feature name.
     * 
     * @return A string representing the extracted feature name.
     *
     * @note The method assumes feature names are located after the last folder separator.
     */
    string extractFeatureName(string path);

    /**
     * @brief Extracts the directory path from a full file path.
     * 
     * @param[in] fullPath The full file path string.
     * 
     * @return A string representing the path without the filename.
     *
     * @note Example: "C:/data/image.jpg" → "C:/data/"
     */
    string extractPath(string fullPath);
};
