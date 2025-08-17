#include "Tester.h"

Tester::Tester(char* a, char* b, char* c, Mode mode) {
    if (mode == EXTRACT) {
        inputPath = a; // Path to the image database  
        selectedMethod = b; // Method for feature extraction (e.g., "SIFT", "SURF", "ORB")   
        vocabularySize = atoi(c); // Size of the vocabulary for indexing
    }
    else {
        queryFolderPath = a; // Path to the query image
        indexPath = b; // Path to the image database
        kTop = atoi(c); // Number of top results to return
    }
}

void Tester::writeExtractionResultToFile(char* Path, char* Method, int vocabulary, string filename) {
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
	log << "Vocabulary Size: " << vocabularySize << "\n";
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

    size_t lastSlash = indexPath.find_last_of("\\/");
    if (lastSlash == string::npos)
        string vocabulary = indexPath; // No slashes at all — assume entire string is feature name

    string vocabulary = indexPath.substr(lastSlash + 1);

    string indexPath = IndexPath;
    string queryPath = queryImage;
    int kTop = k;

    //log << "Feature Extraction Test with:\n";
    //log << "Query Image: " << queryPath << "\n";
    log << "Feature: " << utils.extractFeatureName(indexPath) << "\n";
	log << "Vocabulary Size:" << vocabulary << "\n";
    log << "kTop " << kTop << "\n";
    log << "Run time: " << queryExecutionTimes << " seconds" << "\n";
  //  for (int i = 0; i < APs.size(); ++i) {
		//log << "Average Precision for query " << i + 1 << ": " << APs[i] << "\n";
  //  }
	log << "Mean Average Precision (mAP): " << mAP << "\n";
    log << "---------------------------------\n";

    log.close();
}

void Tester::runTestFeatureExtraction() {
    timer.start();
    if (!inputPath.empty()) {
        imagedatabase.readImageDatabase(inputPath, log);
        indexer.indexingImageDatabase(inputPath, selectedMethod, imagedatabase, log, vocabularySize);
    }
    timer.stop(); 
    elapsedTimes = timer.elapsedSeconds();
}

void Tester::runTestQuery() {
    indexer.readIndex(indexPath);
    map<string, Feature*> features = indexer.getFeatures();
    Mat vocabulary = indexer.getVocab();

    cout << "Getting started" << endl;
    cout << "Feature size: " << features.size() << endl;

	selectedMethod = utils.extractFeatureName(indexPath);

    // Get all image file names in the query folder
    vector<String> imageFiles;

    vector<string> fn_jpg, fn_png, fn;

    // Search for all .jpg files in the directory
    String directory_jpg = queryFolderPath + "/*.jpg";
    log.writeToImageDatabaseLog("Opening " + directory_jpg);
    glob(directory_jpg, fn_jpg, false);

    // Search for all .png files in the directory
    String directory_png = queryFolderPath + "/*.png";
    log.writeToImageDatabaseLog("Opening " + directory_png);
    glob(directory_png, fn_png, false);

    // Merge both .jpg and .png file paths into a single vector
    fn.insert(fn.end(), fn_jpg.begin(), fn_jpg.end());
    fn.insert(fn.end(), fn_png.begin(), fn_png.end());

    size_t count = fn.size(); // Total number of images found

    if (count == 0) {
        log.writeToImageDatabaseLog("No images found in the folder.");
        return;
    }

    timer.start();

    for (size_t i = 0; i < count; i++) {
        // Extract image ID
        size_t lastSlash = fn[i].find_last_of("\\/");
        string filename = fn[i].substr(lastSlash + 1);
        size_t lastDot = filename.find_last_of('.');
        string nameWithoutExt = filename.substr(0, lastDot);

        if (nameWithoutExt.length() == 5) {
			nameWithoutExt = nameWithoutExt.substr(0, 3); // Adjust if ID is 5 characters
        }

        // Load image
        Mat img = imread(fn[i]);
        if (img.empty()) {
            cout << "Failed to load image: " << fn[i] << endl;
            continue;
        }

        // Assign and run search
        Image queryImage;
        queryImage.assignImg(nameWithoutExt, img);
        query.Search(queryImage.getId(), queryImage.getImg(), features, vocabulary, kTop, selectedMethod);
        vector<pair<string, float>> results = query.getResult();

        // Evaluate this query
        evaluator.calculateAveragePrecision(results, queryImage.getId(), features);
    }

    // Calculate final mAP
    evaluator.calculateMeanAveragePrecision();
    timer.stop();
    queryExecutionTimes = timer.elapsedSeconds();
    mAP = evaluator.getMAP();
    APs = evaluator.getAP();
}