#include "Evaluate.h"

void Evaluator::calculateAveragePrecision(vector<pair<string, float>> retrievedList, string query_image) {
    int relevantFound = 0;
    double precisionSum = 0.0;

    for (size_t i = 0; i < retrievedList.size(); ++i) {
        if (retrievedList[i].first.find(query_image) != string::npos) {
            cout << retrievedList[i].first << " vs " << query_image << endl;
            ++relevantFound;
            precisionSum += static_cast<double>(relevantFound) / (i + 1);
            cout << precisionSum;
        }
    }

    double averagePrecision = 0.0;
    if (relevantFound > 0) {
        averagePrecision = precisionSum / relevantFound;
    }

    averagePrecisions.push_back(averagePrecision);

    cout << "AP for " << query_image << ": " << fixed << setprecision(4) << averagePrecision << endl;
}

void Evaluator::calculateMeanAveragePrecision() {
    double sumAP = 0.0;
    for (double ap : averagePrecisions) {
        sumAP += ap;
    }
    mAP = (averagePrecisions.empty()) ? 0.0 : sumAP / averagePrecisions.size();

    cout << "Mean Average Precision (mAP): " << fixed << setprecision(4) << mAP << endl;
}

float Evaluator::getMAP() {
    return mAP;
}