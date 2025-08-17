#pragma once

#include <string>
#include "UI.h"

/**
 * @enum Mode
 * @brief Specifies the mode for the Tester: either feature extraction or query.
 */
enum Mode { EXTRACT, QUERY };

/**
 * @class Tester
 * @brief A utility class to test feature extraction and image retrieval processes.
 *
 * This class encapsulates the full testing pipeline for both the feature extraction phase
 * and the query/evaluation phase, including indexing, similarity calculation, timing,
 * logging, and performance evaluation such as mAP.
 */
class Tester {
private:
    string inputPath;           ///< Path to the input image folder or query folder
    string selectedMethod;      ///< Feature extraction method (e.g., "SIFT", "ORB", "HOG")
    string queryFolderPath;     ///< Path to the query image folder (used in QUERY mode)
    string indexPath;           ///< Path to the indexed features folder (used in QUERY mode)
    int kTop;                   ///< Number of top retrieval results to return
    int vocabularySize;         ///< Size of the visual vocabulary for BoVW-based indexing

    double elapsedTimes;         ///< Time taken for feature extraction
    double queryExecutionTimes; ///< Time taken for query execution
    double mAP;                 ///< Mean Average Precision across all queries
    vector<double> APs;         ///< List of Average Precisions for each query image

    Log log;                    ///< Logger for recording status messages
    Query query;                ///< Query executor
    Timer timer;                ///< Timer for benchmarking
    Indexer indexer;            ///< Indexing utility
    ImageDatabase imagedatabase;///< Image loader and manager
    Evaluator evaluator;        ///< Evaluation utility (for computing AP, mAP)
    Utils utils;                ///< Helper functions

public:
    /**
     * @brief Constructor for Tester class.
     *
     * @param[in] a      The first argument, interpreted as input or query path.
     * @param[in] b      The second argument, interpreted as method or index path.
     * @param[in] c      The third argument, interpreted as vocabulary size or kTop.
     * @param[in] mode   Mode of operation: EXTRACT or QUERY.
     */
    Tester(char* a, char* b, char* c, Mode mode);

    /**
     * @brief Writes feature extraction summary and performance results to file.
     *
     * @param[in] inputPath         Path to the dataset.
     * @param[in] method            Feature extraction method.
     * @param[in] vocabSize         Size of the vocabulary used.
     * @param[in] filenamePrefix    Output filename prefix.
     * 
     * @return void
     */
    void writeExtractionResultToFile(char* inputPath, char* method, int vocabSize, string filenamePrefix);

    /**
     * @brief Writes query results including ranks, scores, and mAP to file.
     *
     * @param[in] queryPath         Path to the query set.
     * @param[in] indexPath         Path to the index of features.
     * @param[in] kTop              Number of top results considered.
     * @param[in] filenamePrefix    Output filename prefix.
     * 
     * @return void
     */
    void writeQueryResultToFile(char* queryPath, char* indexPath, int kTop, string filenamePrefix);

    /**
     * @brief Runs the feature extraction and indexing pipeline.
     *
     * This function loads images, extracts features using the selected method,
     * builds the BoVW vocabulary, indexes the features, and logs the process.
     * 
     * @return void
     */
    void runTestFeatureExtraction();

    /**
     * @brief Runs the image retrieval process.
     *
     * Loads query images, computes their feature vectors, performs retrieval,
     * ranks the results, and evaluates using Average Precision and mAP.
     * 
     * @return void
     */
    void runTestQuery();
};
