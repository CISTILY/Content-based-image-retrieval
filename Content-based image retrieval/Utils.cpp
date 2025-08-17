#include "Utils.h"

string Utils::extractFileName(string filePath) {
    size_t lastSlash = filePath.find_last_of("\\/");
    if (lastSlash == string::npos)
        return filePath; // No slashes at all — assume entire string is feature name

    string lastComponent = filePath.substr(lastSlash + 1);

    // Check if the last component is numeric (vocabulary size)
    bool isNumber = !lastComponent.empty() && all_of(lastComponent.begin(), lastComponent.end(), ::isdigit);

    if (isNumber) {
        // Go back one more level
        string pathWithoutLast = filePath.substr(0, lastSlash);
        size_t secondLastSlash = pathWithoutLast.find_last_of("\\/");
        if (secondLastSlash == string::npos)
            return pathWithoutLast; // Only one folder level
        return pathWithoutLast.substr(secondLastSlash + 1); // This is the feature name
    }
    else {
        return lastComponent; // The last component is already the feature name
    }
}

string Utils::extractPath(string filePath) {
    size_t lastSlash = filePath.find_last_of("\\/");
    if (lastSlash == string::npos)
        return filePath; // No directory separator found
    return filePath.substr(0, lastSlash + 1); // Include the slash
}

string Utils::extractFeatureName(string path) {
    size_t lastSlash = path.find_last_of("\\/");
    if (lastSlash == string::npos)
        return path; // No slashes at all — assume entire string is feature name

    string lastComponent = path.substr(lastSlash + 1);

    // Check if the last component is numeric (vocabulary size)
    bool isNumber = !lastComponent.empty() &&
        all_of(lastComponent.begin(), lastComponent.end(), ::isdigit);

    if (isNumber) {
        // Go back one more level
        string pathWithoutLast = path.substr(0, lastSlash);
        size_t secondLastSlash = pathWithoutLast.find_last_of("\\/");
        if (secondLastSlash == string::npos)
            return pathWithoutLast; // Only one folder level
        return pathWithoutLast.substr(secondLastSlash + 1); // This is the feature name
    }
    else {
        return lastComponent; // The last component is already the feature name
    }
}