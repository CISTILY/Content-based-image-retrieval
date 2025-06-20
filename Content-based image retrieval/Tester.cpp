#include "Tester.h"

Tester::Tester(char* Path, char* Method) {
	inputPath = Path; // Path to the image database  
	selectedMethod = Method; // Method for feature extraction (e.g., "SIFT", "SURF", "ORB")   
}

Tester::Tester(char* QueryImage, char* Path, char* k) {
    indexPath = Path; // Path to the image database  
    queryFolderPath = QueryImage; // Path to the query image     
    kTop = atoi(k); // Number of top results to return
}

void Tester::writeExtractionResultToFile(char* Path, char* Method, string filename) {
    ofstream log(filename + ".txt", ios::app); // append mode

    if (!log.is_open()) {
        cout << "Unable to open log file.\n";
        return;
    }

    string imageDatabasePath = Path;
    string selectedMethod = Method;
    
    log << "Feature Extraction Test with:\n";
    log << "Image Database: " << imageDatabasePath << "\n";
    log << "Selected Method: " << selectedMethod << "\n";
    log << "Run time: " << elapsedTimes << " seconds" << "\n";
    log << "---------------------------------\n";

    log.close();
}

void Tester::writeQueryResultToFile(char* queryImage, char* IndexPath, int k, string filename) {
    ofstream log(filename + ".txt", ios::app); // append mode

    if (!log.is_open()) {
        cout << "Unable to open log file.\n";
        return;
    }

    string indexPath = IndexPath;
    string queryPath = queryImage;
    int kTop = k;

    log << "Feature Extraction Test with:\n";
    log << "Query Image: " << queryPath << "\n";
    log << "Index: " << indexPath << "\n";
    log << "kTop " << kTop << "\n";
    log << "Run time: " << queryExecutionTimes << " seconds" << "\n";
	log << "Mean Average Precision (mAP): " << mAP << "\n";
    log << "---------------------------------\n";

    log.close();
}

void Tester::runTestFeatureExtraction() {
    timer.start();
    if (!inputPath.empty()) {
        imagedatabase.readImageDatabase(inputPath, log);
        indexer.indexingImageDatabase(inputPath, selectedMethod, imagedatabase, log);
    }
    timer.stop();
    elapsedTimes = timer.elapsedSeconds();
}

void Tester::runTestQuery() {
    indexer.readIndex(indexPath);
    map<int, vector<string>> index = indexer.getIndex();
    map<int, vector<float>> centroids = indexer.getCentroid();
    map<int, vector<Feature*>> features = indexer.getFeatures();
    Mat vocabulary = indexer.getVocab();

    cout << "Getting started" << endl;
    cout << "Index size: " << index.size() << endl;
    cout << "Centroid size: " << centroids.size() << endl;
    cout << "Feature size: " << features.size() << endl;

    size_t lastSlash = indexPath.find_last_of("\\/");
    selectedMethod = (lastSlash == string::npos) ? "" : indexPath.substr(lastSlash + 1);

    // Get all image file names in the query folder
    vector<String> imageFiles;
    glob(queryFolderPath + "/*.jpg", imageFiles, false);  // you can add png or other formats too

    if (imageFiles.empty()) {
        cout << "No query images found in folder: " << queryFolderPath << endl;
        return;
    }

    timer.start();

    for (const auto& imagePath : imageFiles) {
        // Extract image ID
        size_t lastSlash = imagePath.find_last_of("\\/");
        string filename = imagePath.substr(lastSlash + 1);
        size_t lastDot = filename.find_last_of('.');
        string nameWithoutExt = filename.substr(0, lastDot);

        if (nameWithoutExt.length() == 5) {
			nameWithoutExt = nameWithoutExt.substr(0, 3); // Adjust if ID is 5 characters
        }

        // Load image
        Mat img = imread(imagePath);
        if (img.empty()) {
            cout << "Failed to load image: " << imagePath << endl;
            continue;
        }

        // Assign and run search
        Image queryImage;
        queryImage.assignImg(nameWithoutExt, img);
        query.Search(queryImage.getId(), queryImage.getImg(), index, centroids, features, vocabulary, kTop, selectedMethod);
        vector<pair<string, float>> results = query.getResult();

        // Evaluate this query
        evaluator.calculateAveragePrecision(results, queryImage.getId());
    }

    // Calculate final mAP
    evaluator.calculateMeanAveragePrecision();
    timer.stop();
    queryExecutionTimes = timer.elapsedSeconds();
    mAP = evaluator.getMAP();
}