#pragma once

#include <vector>

#include "ImageDatabase.h"

using namespace std;

/**
 * @class Evaluator
 * @brief Evaluates retrieval performance using Average Precision (AP) and Mean Average Precision (mAP).
 *
 * This class accumulates per-query average precisions and computes the mean average precision
 * over a set of retrieval results. It is commonly used in content-based image retrieval systems.
 */
class Evaluator {
private:
    vector<double> averagePrecisions; ///< Stores the average precision for each query.
    double mAP;                       ///< Mean Average Precision over all queries.

public:
    /**
     * @brief Default destructor.
     */
    ~Evaluator() {}

    /**
     * @brief Calculates the Average Precision (AP) for a single query.
     * @param rankedList The ranked list of retrieved items (image ID, similarity/distance score).
     * @param queryId The ID of the query image (used to determine relevance).
     *
     * This method assumes that relevance is determined by matching image IDs
     * (e.g., the same class prefix or exact match). The precision is calculated
     * at each relevant hit in the ranked list.
     */
    void calculateAveragePrecision(vector<pair<string, float>> rankedList, string queryId);

    /**
     * @brief Computes the Mean Average Precision (mAP) over all queries.
     *
     * This is calculated as the average of all stored average precision values.
     * Should be called after running `calculateAveragePrecision()` for each query.
     */
    void calculateMeanAveragePrecision();

    /**
     * @brief Returns the current mean average precision value.
     * @return The computed mAP value.
     */
    float getMAP();
};
