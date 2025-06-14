#include "Distances.h"

float Distance::calculateSimilarity(Mat query, Mat image, string type) {
    if (type == "Cosine") {
        // Ensure the input is a single row (flattened vector)
        if (query.rows != 1 || image.rows != 1 || query.cols != image.cols || query.type() != image.type()) {
            cout << "Error: Input vectors must be 1D, same length and type." << endl;
            return 0.0f;
        }

        float dot_product = 0.0f;
        float norm_a = 0.0f;
        float norm_b = 0.0f;

        // Use the float pointer to iterate
        const float* query_ptr = query.ptr<float>();
        const float* image_ptr = image.ptr<float>();

        for (int i = 0; i < query.cols; ++i) {
            dot_product += query_ptr[i] * image_ptr[i];
            norm_a += query_ptr[i] * query_ptr[i];
            norm_b += image_ptr[i] * image_ptr[i];
        }

        norm_a = sqrt(norm_a);
        norm_b = sqrt(norm_b);

        if (norm_a == 0.0f || norm_b == 0.0f) {
            if (norm_a == 0.0f) cout << "Warning: norm_a is 0" << endl;
            if (norm_b == 0.0f) cout << "Warning: norm_b is 0" << endl;
            return 0.0f;
        }

        return dot_product / (norm_a * norm_b);
    }
    else {
        cout << "Error: Unsupported similarity type '" << type << "'" << endl;
        return 0.0f;
    }
}

float Distance::calculateDistance(Mat query, Mat image, string type) {
    if (type == "L2") {
        float sim = norm(query, image, NORM_L2);
        return sim;
    }
}