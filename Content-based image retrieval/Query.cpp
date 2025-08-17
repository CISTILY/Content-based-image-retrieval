#include "Query.h"

void Query::Search(string image_id, Mat query,
    map<string, Feature*>& features,
    Mat& vocabulary,
    int kTop, string extractMethod)
{
    Distance distance;
    Feature* feature = nullptr;
    results.clear();
    cout << "Querying" << endl;

    // === Feature selection ===
    if (extractMethod == "Color Histogram") {
        feature = new ColorHistogram;
        useSimilarity = true;
    }
    else if (extractMethod == "Color Correlogram") {
        feature = new ColorCorrelogram;
        useSimilarity = true;
    }
    else if (extractMethod == "HOG") {
        feature = new HOG;
        useSimilarity = false;
    }
    else if (extractMethod == "SIFT") {
        feature = new SIFTFeature;
        useSimilarity = false;
    }
    else if (extractMethod == "ORB") {
        feature = new ORBFeature;
        useSimilarity = false;
    }
    else {
        cerr << "Unsupported feature type!" << endl;
        return;
    }

    // === Feature extraction ===
    Image img;
    img.assignImg(image_id, query);
    feature->createFeature(img.getId(), img.getImg());
    cout << "Query image feature extraction done" << endl;

    // === Convert to BoVW if local feature ===
    if ((extractMethod == "SIFT" || extractMethod == "ORB" || extractMethod == "HOG") && !vocabulary.empty()) {
        const Mat& localDescriptors = feature->getDescriptor();
        if (!localDescriptors.empty()) {
            BagOfVisualWord bovw(vocabulary);
            Mat hist = bovw.computeHistogram(localDescriptors);
            feature->setDescriptor(hist);
        }
    }

    const Mat& queryDescriptor = feature->getDescriptor();
    if (queryDescriptor.empty()) {
        cerr << "Query descriptor is empty!" << endl;
        delete feature;
        return;
    }

    // === Search all features ===
    vector<pair<string, float>> distances;
    for (const auto& [imgId, f] : features) {
        const Mat& featDescriptor = f->getDescriptor();
        if (featDescriptor.empty())
            cout << "No descriptor found" << endl;

        float score = 0;
        string type;

        if (useSimilarity) {
            type = "Chi-square";
            score = distance.calculateSimilarity(queryDescriptor, featDescriptor, type);
        }
        else {
            type = "L2";
            score = distance.calculateDistance(queryDescriptor, featDescriptor, type);
        }

        distances.emplace_back(imgId, score);
    }

    // === Sort results ===
    if (useSimilarity) {
        std::sort(distances.begin(), distances.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });  // Higher = better
    }
    else {
        std::sort(distances.begin(), distances.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });  // Lower = better
    }

    // === Output top-k results ===
    for (int i = 0; i < std::min(kTop, static_cast<int>(distances.size())); ++i) {
        cout << "ID: " << distances[i].first << " score: " << distances[i].second << endl;
        results.push_back(distances[i]);
    }

    delete feature;
}


vector<pair<string, float>> Query::getResult() {
	return results;
}
