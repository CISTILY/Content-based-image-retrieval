#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

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
    Mat vocabulary;
    map<int, vector<string>> index;         ///< Maps cluster ID to list of image IDs belonging to that cluster
    map<int, vector<float>> centroids;      ///< Maps cluster ID to its centroid vector
    map<int, vector<Feature*>> clusters;    ///< Maps cluster ID to list of Feature pointers in that cluster

public:
    /**
     * @brief Default constructor.
     */
    Indexer() {}

    /**
     * @brief Destructor for the Indexer class.
     *
     * Responsible for releasing dynamically allocated memory held by Feature pointers
     * stored in the `clusters` map. Each feature object is deleted to prevent memory leaks.
     * After deletion, the cluster map is cleared for proper cleanup.
     */
    ~Indexer();

    /**
     * @brief Performs feature extraction and indexing for the entire image database.
     *
     * This function extracts visual features from all images in the database using the specified
     * feature extraction method, clusters the features, and creates a searchable index saved to disk.
     *
     * @param[in] imageDatabasePath   Path to the image database (used to locate or create the extracted_feature folder).
     * @param[in] selectedFeature     The selected feature extraction method (e.g., "SIFT", "HOG", etc.).
     * @param[in] imageDatabase       The ImageDatabase object containing loaded image data.
     * @param[in,out] log             Logging utility used to record the indexing process.
     *
     * @return void
     *
     * @note The function performs feature extraction, k-means clustering, and writes the resulting
     *       index and cluster centroids to binary files for later retrieval.
     */
    void indexingImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase database, Log& log);

    /**
     * @brief Extract features from all images in the image database using the selected method.
     *
     * This function iterates over all images in the provided image database and applies the specified
     * feature extraction technique (e.g., Color Histogram, HOG, SIFT, ORB, etc.). The resulting
     * feature objects are stored in the `extractedFeatures` vector.
     *
     * @param[in] imageDatabasePath      Path to the image database folder (not used here but can be useful for logging or future expansion).
     * @param[in] selectedFeature        The selected feature extraction method (e.g., "SIFT", "HOG", "Color Histogram").
     * @param[in] database               The ImageDatabase object containing all loaded images.
     * @param[out] extractedFeatures     Vector to store the extracted features from each image.
     * @param[in,out] log                Logging object used to record status or debug messages (not used directly in this version).
     *
     * @retun void
     *
     * @note The function only support Color Histogram, Color correlogram, SIFT, ORB feature extraction method
     */
    void extractFeatureImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase database, vector<Feature*>& extractedFeatures, Log& log);

    /**
     * @brief Create and save a cluster-based feature index from extracted image features.
     *
     * This function groups extracted image features into clusters using the provided labels and centroids.
     * It serializes and saves the resulting cluster index—including cluster centroids, feature mappings, and
     * feature data—to a binary file. The index enables efficient retrieval or matching of image features
     * in future operations.
     *
     * @param[in] indexPath             The base path where the index file should be saved.
     * @param[in] selectedFeature       The selected feature extraction method name (used to build subfolder structure).
     * @param[in] extractedFeatures     A vector of pointers to Feature objects extracted from the image database.
     * @param[in,out] log               Logging object used to record status or debug messages.
     * @param[in] labels                A Mat where each element indicates the cluster ID assigned to a feature.
     * @param[in] centers               A Mat containing centroid vectors for each cluster (rows = clusters, cols = dimensions).
     *
     * @return true if the index was successfully created and saved; false if an error occurred during file writing.
     *
     * @note The function assumes clustering has already been performed and uses the output (labels and centers)
     *       to organize and store feature data. The final index file is written in a custom binary format.
     */
    bool saveIndex(string indexPath, string selectedFeature, vector<Feature*>& extractedFeatures, Log& log, Mat labels, Mat centers);

    /**
     * @brief Perform k-means clustering on feature vectors (either global or local).
     *
     * This function takes a list of feature objects and groups them into `clusterCount` clusters
     * using the OpenCV k-means algorithm. It supports both global and local feature types.
     * The resulting cluster labels and centroids are stored in the provided `labels` and `centers` matrices.
     *
     * @param[in] features      Vector of pointers to Feature objects (must contain either global or local features).
     * @param[in] clusterCount  Desired number of clusters.
     * @param[out] labels       Output labels for each sample used in k-means (CV_32S type).
     * @param[out] centers      Output cluster centers (centroids), as a CV_32F matrix.
     *
     * @note - If features contain only local descriptors, all descriptors from all features are clustered.
     *       - If features contain global descriptors, one global vector per feature is used.
     *       - Clustering is skipped if the feature list is empty or contains no valid descriptors.
     */
    void clustering(vector<Feature*>& features, int clusterCount, Mat& labels, Mat& centers);

    /**
     * @brief Extract the directory path from a full file path.
     *
     * This function finds the last directory separator ('/' or '\\') in the path
     * and returns the substring up to and including that separator. If no separator is found,
     * the entire input string is returned as-is.
     *
     * @param[in] filePath Full path to a file (e.g., "C:/data/image.jpg").
     *
     * @return string Directory path (e.g., "C:/data/").
     */
    string extractPath(string filePath);

    /**
     * @brief Extract the file name from a full file path.
     *
     * This function finds the last directory separator ('/' or '\\') in the path
     * and returns the substring that follows it. If no separator is found,
     * the entire input string is returned.
     *
     * @param[in] filePath Full path to a file (e.g., "C:/data/image.jpg").
     *
     * @return string File name (e.g., "image.jpg").
     */
    string extractFileName(string filePath);
    
    /**
     * @brief Create a folder (including parent directories) if it does not already exist.
     *
     * This function checks whether the specified folder path exists on the file system.
     * If it does not, it attempts to create the folder and any necessary intermediate directories.
     * Messages are printed to the console indicating whether the folder was created or already existed.
     *
     * @param[in] folderPath   The path to the folder to check or create.
     *
     * @return true if the folder exists or was successfully created; false if an error occurred during creation.
     *
     * @note This function uses the C++17 filesystem library. It handles filesystem exceptions
     *       and provides basic diagnostic messages to standard output and error.
     */
    bool createFolderIfNotExists(string folderPath);

    /**
     * @brief Read a precomputed cluster index from a binary file.
     *
     * This function reads a previously saved cluster index from disk. It reconstructs the in-memory
     * data structures for centroids, index mappings (cluster IDs to image IDs), and extracted feature
     * vectors (either global or local). The function also handles dynamic instantiation of appropriate
     * Feature subclasses based on the feature type.
     *
     * @param[in] indexPath   Path to the directory containing the binary index file ("index.bin").
     *
     * @return true if the index was successfully read and reconstructed; false if the file could not be opened.
     *
     * @note The function assumes the binary file follows the format written by createIndex().
     *       It supports the following feature types: "Color Histogram", "Color Correlogram", "SIFT", "ORB".
     */
    bool readIndex(string indexPath);

    /**
     * @brief Get the image index mapping.
     *
     * Returns a map where each cluster ID maps to a list of image IDs
     * that belong to that cluster.
     *
     * @return map<int, vector<string>> Cluster ID to image ID list mapping.
     */
    map<int, vector<string>> getIndex();

    /**
     * @brief Get the centroid vectors of all clusters.
     *
     * Returns a map where each cluster ID maps to its centroid feature vector.
     *
     * @return map<int, vector<float>> Cluster ID to centroid vector mapping.
     */
    map<int, vector<float>> getCentroid();

    /**
     * @brief Get the clustered feature pointers.
     *
     * Returns a map where each cluster ID maps to a list of Feature pointers
     * that were assigned to that cluster.
     *
     * @return map<int, vector<Feature*>> Cluster ID to list of Feature pointers.
     */
    map<int, vector<Feature*>> getFeatures();

    Mat getVocab();
};