#include "Distances.h"

float Distance::calculateSimilarity(Mat query, Mat image, string type) {
    if (type == "Chi-square") {
        // Ensure both histograms are of the same size and type
        CV_Assert(query.size() == image.size());
        CV_Assert(query.type() == CV_32F && image.type() == CV_32F);

        double chi2 = 0.0;

        // Loop through each histogram bin
        for (int i = 0; i < query.total(); ++i) {
            float h1 = query.at<float>(i);
            float h2 = image.at<float>(i);

            // Avoid division by zero
            if (h1 + h2 != 0.0f) {
                chi2 += ((h1 - h2) * (h1 - h2)) / (h1 + h2);
            }
        }

        chi2 *= 0.5;

        // Convert Chi-square distance to a similarity score in [0, 1]
        double similarity = 1.0 / (1.0 + chi2); // As chi2 → 0, similarity → 1
        return similarity;
    }
    else {
        // Unsupported similarity type
        cout << "Error: Unsupported similarity type '" << type << "'" << endl;
        return 0.0f;
    }
}

float Distance::calculateDistance(Mat query, Mat image, string type) {
    if (type == "L2") {
        // Euclidean (L2) distance between the two vectors
        float sim = norm(query, image, NORM_L2);
        return sim;
    }
    else if (type == "Hamming") {
        // Hamming distance – assumes binary vectors stored in float format
        CV_Assert(query.cols == image.cols && query.type() == CV_32F && image.type() == CV_32F);

        int distance = 0;

        for (int i = 0; i < query.cols; ++i) {
            // Convert float to uchar to perform XOR operation
            uchar xorByte = static_cast<uchar>(query.at<float>(0, i)) ^ static_cast<uchar>(image.at<float>(0, i));
            distance += countNonZero(Mat(1, 1, CV_8U, xorByte));
        }

        return distance;
    }
    else {
        // Unsupported distance type
        cout << "Error: Unsupported distance type '" << type << "'" << endl;
        return -1.0f;
    }
}
