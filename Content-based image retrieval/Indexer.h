#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

#include "Utils.h"
#include "Features.h"
#include "ImageDatabase.h"
#include "BoVW.h"

namespace fs = filesystem;

/**
 * @class Indexer
 * @brief Handles feature extraction, clustering, indexing, and storage of image features.
 *
 * The Indexer class supports generating feature indexes for an image database using
 * various feature extraction methods (global or local). It also provides functionality
 * to cluster features, store/retrieve them, and manage index-related operations.
 */
class Indexer {
private:
    Utils utils;                        ///< Utility functions for common operations
    Mat vocabulary;                     ///< Vocabulary (cluster centers) used for BoVW
    map<string, Feature*> features;     ///< Map of feature IDs to their corresponding Feature pointers

public:
    /**
     * @brief Default constructor.
     */
    Indexer() {}

    /**
     * @brief Destructor for the Indexer class.
     *
     * Releases dynamically allocated memory held by Feature pointers
     * stored in the `features` map. Prevents memory leaks by deleting each object.
     */
    ~Indexer();

    /**
     * @brief Performs feature extraction and indexing for the entire image database.
     *
     * This function extracts visual features from all images in the database using the specified
     * method, performs k-means clustering, and saves the resulting index to disk.
     *
     * @param[in] imageDatabasePath   Path to the image database (used to locate or create the extracted_feature folder).
     * @param[in] selectedFeature     The feature extraction method to use (e.g., "SIFT", "HOG").
     * @param[in] imageDatabase       The ImageDatabase object containing loaded images.
     * @param[in,out] log             Logging utility for recording indexing process details.
     * @param[in] vocabularySize      The number of clusters (visual words) to use in BoVW.
     */
    void indexingImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase database, Log& log, int vocabularySize);

    /**
     * @brief Extract features from all images using the selected method.
     *
     * Iterates through all images and applies the given feature extraction method
     * (e.g., Color Histogram, HOG, SIFT, ORB). Stores extracted features for indexing.
     *
     * @param[in] imageDatabasePath   Path to the image database folder.
     * @param[in] selectedFeature     Feature extraction method (e.g., "SIFT", "ORB").
     * @param[in] database            The ImageDatabase containing loaded images.
     * @param[out] extractedFeatures  Vector to store extracted features from each image.
     * @param[in,out] log             Logger for process feedback.
     * @param[in] dictionarySize      The number of clusters (visual words) to generate.
     *
     * @return void
     *
     * @note Supported methods include: "Color Histogram", "Color Correlogram", "SIFT", "HOG", and "ORB".
     */
    void extractFeatureImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase database, vector<Feature*>& extractedFeatures, Log& log, int dictionarySize);

    /**
     * @brief Save clustered features as an index to disk.
     *
     * Groups features into clusters and serializes the index (with cluster centers and mappings)
     * to disk, enabling later retrieval and matching.
     *
     * @param[in] indexPath          Destination folder for the index file.
     * @param[in] selectedFeature    Feature extraction method used.
     * @param[in] extractedFeatures  List of extracted Feature pointers.
     * @param[in,out] log            Logger to record progress.
     * @param[in] dictionarySize     Number of clusters used during BoVW indexing.
     *
     * @return true if index was saved successfully; false otherwise.
     */
    bool saveIndex(string indexPath, string selectedFeature, vector<Feature*>& extractedFeatures, Log& log, int dictionarySize);

    /**
     * @brief Create a folder and any parent directories if they don’t exist.
     *
     * Checks and creates the specified folder path.
     *
     * @param[in] folderPath The target folder path.
     * @return true if the folder exists or was created successfully; false otherwise.
     */
    bool createFolderIfNotExists(string folderPath);

    /**
     * @brief Load a saved index from disk.
     *
     * Reads a binary index file and reconstructs the in-memory structure for
     * the BoVW vocabulary and feature mappings.
     *
     * @param[in] indexPath Path to the directory containing the saved index.
     * @return true if index was loaded successfully; false otherwise.
     *
     * @note Supported types: "Color Histogram", "Color Correlogram", "SIFT", "HOG", and "ORB".
     */
    bool readIndex(string indexPath);

    /**
     * @brief Get the map of extracted features.
     * 
     * @return A map from feature ID to Feature pointer.
     */
    map<string, Feature*> getFeatures();

    /**
     * @brief Get the BoVW vocabulary (cluster centers).
     * 
     * @return A matrix of cluster centers.
     */
    Mat getVocab();
};
