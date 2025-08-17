#include "Evaluate.h"

void Evaluator::calculateAveragePrecision(vector<pair<string, float>> retrievedList, string query_image, map<string, Feature*> groundTruth) {
    int relevantFound = 0;         // Number of relevant images found so far
	int groundTruthSize = 0;           // Total number of relevant images in the ground truth
    double precisionSum = 0.0;     // Sum of precision values at each relevant position

	// Construting groundth truth vector for the query image
    for (const auto& entry : groundTruth) {
        string id = entry.first;  // this is the image ID (key)

        // Check if the ID contains the query image ID as substring
        if (id.find(query_image) != string::npos) {
            ++groundTruthSize;
        }
    }

    for (size_t i = 0; i < retrievedList.size(); ++i) {
        // Determine if current result is relevant by checking if query ID is part of the result ID
        if (retrievedList[i].first.find(query_image) != string::npos) {
            cout << retrievedList[i].first << " vs " << query_image << endl;

            ++relevantFound;

            // Precision at current rank = relevantFound / (i + 1)
            precisionSum += static_cast<double>(relevantFound) / (i + 1);
            cout << precisionSum;
        }
    }

    double averagePrecision = 0.0;

	cout << "Ground truth relevant images: " << groundTruthSize << endl;
	cout << "Relevant images found: " << relevantFound << endl;

    // Compute average precision if any relevant results were found
    if (groundTruthSize > 0) {
        averagePrecision = precisionSum / groundTruthSize;
    }

    // Store the AP value for later mAP computation
    averagePrecisions.push_back(averagePrecision);

    cout << "AP for " << query_image << ": " << fixed << setprecision(4) << averagePrecision << endl;
}

void Evaluator::calculateMeanAveragePrecision() {
    double sumAP = 0.0;

    // Sum all stored average precision values
    for (double ap : averagePrecisions) {
        sumAP += ap;
    }

    // Compute mAP as the average of APs
    mAP = (averagePrecisions.empty()) ? 0.0 : sumAP / averagePrecisions.size();

    cout << "Mean Average Precision (mAP): " << fixed << setprecision(4) << mAP << endl;
}

vector<double> Evaluator::getAP() {
    return averagePrecisions;
}

double Evaluator::getMAP() {
    return mAP;
}
