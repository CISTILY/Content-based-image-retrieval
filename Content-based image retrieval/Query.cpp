#include "Query.h"

void Query::Search(string image_id, Mat query,
    map<int, vector<string>>& index,
    map<int, vector<float>>& centroids,
    map<int, vector<Feature*>>& features,
    Mat& vocabulary,
    int kTop, string extractMethod) {

    Distance distance;

    Feature* feature = nullptr;
    cout << "Querying" << endl;

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
        feature = new SIFTFeature; // now BoVW
        useSimilarity = false;
    }
    else if (extractMethod == "ORB") {
        feature = new ORBFeature;  // now BoVW
        useSimilarity = false;
    }  

    if (!feature) {
        cerr << "Unsupported feature type!" << endl;
        return;
    }

    Image Image;
    Image.assignImg(image_id, query);

    feature->createFeature(Image.getId(), Image.getImg());
    cout << "Query image feature extraction done" << endl;

    // === Convert to BoVW if using local features ===
    if ((extractMethod == "SIFT" || extractMethod == "ORB") && !vocabulary.empty()) {
        const Mat& descriptors = feature->getDescriptor();  // assume this returns local descriptors

        if (!descriptors.empty()) {
            BagOfVisualWord bovw(vocabulary);
            Mat hist = bovw.computeHistogram(feature->getDescriptor());
            feature->setDescriptor(hist);
        }
    }

    const Mat& queryDescriptor = feature->getDescriptor();

    // Step 1: Find nearest cluster centroid
    vector<pair<int, float>> centroidDistances;

    for (const auto& [clusterId, centroidVec] : centroids) {
        Mat centroidMat(1, centroidVec.size(), CV_32F, (void*)centroidVec.data());
        float sim = 0;
        string type;
        if (useSimilarity == true) {
            type = "Chi-square";
            sim = distance.calculateSimilarity(queryDescriptor, centroidMat, type);
            sort(centroidDistances.begin(), centroidDistances.end(),
                [](const pair<int, float>& a, const pair<int, float>& b) {
                    return a.second > b.second;
                });
        }
        else {
            if (extractMethod == "ORB") 
                type = "Hamming";
            else 
                type = "L2";
            sim = distance.calculateDistance(queryDescriptor, centroidMat, type);
            sort(centroidDistances.begin(), centroidDistances.end(),
                [](const pair<int, float>& a, const pair<int, float>& b) {
                    return a.second < b.second;
                });
        }
        cout << "Cluster ID: " << clusterId << " Similarity: " << sim << " Type: " << type << endl;
        centroidDistances.emplace_back(clusterId, sim);
    }

    // Step 2: Search in ranked clusters
    vector<pair<string, float>> distances;
    unordered_set<string> visitedImages;

    for (const auto& [clusterId, _] : centroidDistances) {
        if (index.count(clusterId) == 0 || features.count(clusterId) == 0)
            continue;

        const vector<string>& imgIds = index[clusterId];
        const vector<Feature*>& feats = features[clusterId];

        for (size_t i = 0; i < imgIds.size() && i < feats.size(); ++i) {
            const string& imgId = imgIds[i];
            if (!visitedImages.insert(imgId).second) continue;

            const Mat& featDescriptor = feats[i]->getDescriptor();
            float sim = 0;
            string type;
            if (useSimilarity == true) {
                type = "Chi-square";
                sim = distance.calculateSimilarity(queryDescriptor, featDescriptor, type);
                sort(distances.begin(), distances.end(),
                    [](const pair<string, float>& a, const pair<string, float>& b) {
                        return a.second > b.second;
                    });
            }
            else {
                if (extractMethod == "ORB")
                    type = "Hamming";
                else
                    type = "L2";
                sim = distance.calculateDistance(queryDescriptor, featDescriptor, type);
                sort(distances.begin(), distances.end(),
                    [](const pair<string, float>& a, const pair<string, float>& b) {
                        return a.second < b.second;
                    });
            }
            distances.emplace_back(imgId, sim);
        }

        if ((int)distances.size() >= kTop)
            break;
    }

    // Step 3: Sort and Return Results
    

    for (int i = 0; i < min(kTop, static_cast<int>(distances.size())); ++i) {
        cout << "ID: " << distances[i].first << " score: " << distances[i].second << endl;
        results.push_back(distances[i]);
    }

    delete feature;
}

vector<pair<string, float>> Query::getResult() {
	return results;
}
