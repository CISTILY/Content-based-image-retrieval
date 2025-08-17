#pragma once

#include <vector>
#include "ImageDatabase.h"
#include "Features.h"

using namespace std;

/**
 * @class Evaluator
 * @brief Evaluates retrieval performance using Average Precision (AP) and Mean Average Precision (mAP).
 *
 * This class computes evaluation metrics for image retrieval systems by comparing ranked retrieval results
 * to ground truth relevance. It supports per-query average precision calculation and aggregation to mean average precision.
 */
class Evaluator {
private:
    vector<double> averagePrecisions; ///< Stores the average precision for each query.
    double mAP = 0.0;                       ///< Mean Average Precision over all queries.

public:
    /**
     * @brief Default destructor.
     */
    ~Evaluator() {}

    /**
     * @brief Calculates the Average Precision (AP) for a single query.
     *
     * This function evaluates the ranked retrieval results against the ground truth.
     * It calculates precision at each relevant position and averages over all relevant results.
     *
     * @param[in] rankedList  A ranked list of image results (image ID and distance or similarity score).
     * @param[in] queryId     The ID or prefix of the query image to determine relevance.
     * @param[in] groundTruth Map of ground truth images used for matching relevance (from index).
     *
     * @return void
     *
     * @note Relevance is determined based on substring matching (e.g., category or shared prefix).
     */
    void calculateAveragePrecision(vector<pair<string, float>> rankedList, string queryId, map<string, Feature*> groundTruth);

    /**
     * @brief Computes the Mean Average Precision (mAP) over all evaluated queries.
     *
     * This should be called after all queries have been evaluated using `calculateAveragePrecision()`.
     * It computes the mean value of all stored APs.
     *
     * @return void
     */
    void calculateMeanAveragePrecision();

    /**
     * @brief Returns the current mean average precision (mAP).
     *
     * @return A double representing the mean average precision.
     */
    double getMAP();

    /**
     * @brief Returns the list of individual average precision (AP) values.
     *
     * @return A vector of AP values, one per evaluated query.
     */
    vector<double> getAP();
};
