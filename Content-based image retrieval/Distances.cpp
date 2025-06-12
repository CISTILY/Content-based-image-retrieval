#include "Distances.h"

float Distance::calculateCosineDistance(vector<float> query, vector<float> image) {
    float dot_product = 0.0f;
    float norm_a = 0.0f;
    float norm_b = 0.0f;

    cout << query.size() << " " << image.size() << endl;
    // Compute dot product and norms
    for (size_t i = 0; i < query.size(); ++i) {
        dot_product += query[i] * image[i];
        norm_a += query[i] * query[i];
        norm_b += image[i] * image[i];
    }

    norm_a = sqrt(norm_a);
    norm_b = sqrt(norm_b);

    // Avoid division by zero
    if (norm_a == 0.0f || norm_b == 0.0f) {
        if (norm_a == 0.0f)
            cout << "Warning: norm_a is 0" << endl;
        if (norm_a == 0.0f)
            cout << "Warning: norm_b is 0" << endl;
        return 0.0f;
    }

    return dot_product / (norm_a * norm_b);
}

float Distance::calculateL1Distance(vector<float> a, vector<float> b) {
    if (a.size() != b.size() || a.empty())
        return 0.0f;

    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += fabs(a[i] - b[i]);
    }
    return sum;
}

float Distance::calculateL2Distance(Mat a, Mat b) {
    if (a.empty() || b.empty() || a.size() != b.size() || a.type() != b.type())
        return FLT_MAX;

    Mat diff;
    absdiff(a, b, diff);
    diff = diff.mul(diff);
    return std::sqrt(sum(diff)[0]);
}

float Distance::averageMinL2Distance(Mat desc1, Mat desc2) {
    if (desc1.empty() || desc2.empty() || desc1.cols != desc2.cols || desc1.type() != desc2.type())
        return FLT_MAX;

    float total = 0.0f;
    for (int i = 0; i < desc1.rows; ++i) {
        float minDist = FLT_MAX;
        Mat vec1 = desc1.row(i);

        for (int j = 0; j < desc2.rows; ++j) {
            Mat vec2 = desc2.row(j);
            float dist = calculateL2Distance(vec1, vec2);
            minDist = std::min(minDist, dist);
        }

        total += minDist;
    }

    return total / desc1.rows;
}