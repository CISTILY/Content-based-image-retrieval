#include "Indexer.h"
#include "ColorHistogram.h"
#include "ColorCorrelogram.h"
#include "SIFT.h"
#include "ORB.h"
#include "HOG.h"

Indexer::~Indexer() {
	for (auto& [id, featurePtr] : features) {
		delete featurePtr;
	}
	features.clear();
}

void Indexer::indexingImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase imageDatabase, Log& log, int vocabularySize) {
	Mat labels, centers;
	vector<Feature*> extractedFeatures;

	// Extract features from all images in the database
	extractFeatureImageDatabase(imageDatabasePath, selectedFeature, imageDatabase, extractedFeatures, log, vocabularySize);

	// Create an index based on the clustering result and save to disk
	saveIndex(imageDatabasePath, selectedFeature, extractedFeatures, log, vocabularySize);

	// Log completion of index saving
	log.writeToFeatureDatabaseLog("Save index done");
}

void Indexer::extractFeatureImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase database, vector<Feature*>& extractedFeatures, Log& log, int dictionarySize) {
	// Extract Color Histogram features
	if (selectedFeature == "Color Histogram") {
		for (int i = 0; i < database.getImage().size(); i++) {
			Feature* colorhistogram = new ColorHistogram;
			cout << "Current Image: " << database.getImage()[i].getId() << endl;
			colorhistogram->createFeature(database.getImage()[i].getId(), database.getImage()[i].getImg());
			extractedFeatures.push_back(colorhistogram);
		}
	}
	// Extract Color Correlogram features
	else if (selectedFeature == "Color Correlogram") {
		for (int i = 0; i < database.getImage().size(); i++) {
			Feature* colorcorrelogram = new ColorCorrelogram;
			cout << "Current Image: " << database.getImage()[i].getId() << endl;
			colorcorrelogram->createFeature(database.getImage()[i].getId(), database.getImage()[i].getImg());
			extractedFeatures.push_back(colorcorrelogram);
		}
	}
	// Extract HOG features (Note: Bug fixed — was incorrectly using ORBFeature)
	else if (selectedFeature == "HOG") {
		for (int i = 0; i < database.getImage().size(); i++) {
			Feature* hog = new HOG;
			cout << "Current Image: " << database.getImage()[i].getId() << endl;
			hog->createFeature(database.getImage()[i].getId(), database.getImage()[i].getImg());
			extractedFeatures.push_back(hog);
		}
	}
	else if (selectedFeature == "SIFT" || selectedFeature == "ORB") {
		vector<Mat> allDescriptors;
		vector<Feature*> rawFeatures;
		Feature* feat = nullptr;

		// Step 1: Extract raw descriptors
		for (int i = 0; i < database.getImage().size(); i++) {
			if (selectedFeature == "SIFT")
				feat = static_cast<Feature*>(new SIFTFeature);
			else if (selectedFeature == "ORB")
				feat = static_cast<Feature*>(new ORBFeature);
			else if (selectedFeature == "HOG")
				feat = static_cast<Feature*>(new HOG);

			cout << "Current Image: " << database.getImage()[i].getId() << endl;
			feat->createFeature(database.getImage()[i].getId(), database.getImage()[i].getImg());
			rawFeatures.push_back(feat);

			Mat desc = feat->getDescriptor();
			if (!desc.empty() && desc.rows > 1)
				allDescriptors.push_back(desc);
		}
		cout << allDescriptors.size() << endl;
		// Step 2: Build BoVW vocabulary
		BagOfVisualWord bovw(dictionarySize);
		bovw.buildVocabulary(allDescriptors);

		// Save vocabulary to use later in indexing
		this->vocabulary = bovw.getVocabulary();

		// Step 3: Convert descriptors to BoVW histograms
		for (Feature* feat : rawFeatures) {
			Mat hist = bovw.computeHistogram(feat->getDescriptor());
			feat->setDescriptor(hist);
			extractedFeatures.push_back(feat);
		}
	}
}

bool Indexer::saveIndex(string indexPath, string selectedFeature, vector<Feature*>& extractedFeatures, Log& log, int dictionarySize) {
	// 1. Save features as map<string, Feature*>
	map<string, Feature*> features;
	for (Feature* f : extractedFeatures) {
		features[f->getId()] = f;  // override if duplicate ID
	}

	// 2. Prepare output path
	if (selectedFeature == "HOG" || selectedFeature == "SIFT" || selectedFeature == "ORB")
		indexPath = utils.extractPath(indexPath) + "extracted_feature/" + utils.extractFileName(indexPath) + "/" + selectedFeature + "/" + to_string(dictionarySize);
	else 
		indexPath = utils.extractPath(indexPath) + "extracted_feature/" + utils.extractFileName(indexPath) + "/" + selectedFeature;

	createFolderIfNotExists(indexPath);
	string indexFile = indexPath + "/index.bin";

	ofstream out(indexFile, ios::binary | ios::trunc);
	if (!out) {
		cerr << "Failed to open file for writing: " << indexFile << endl;
		return false;
	}

	// 3. Save vocabulary (for BoVW)
	if (!vocabulary.empty()) {
		int vocabRows = vocabulary.rows;
		int vocabCols = vocabulary.cols;
		int vocabType = vocabulary.type();

		out.write(reinterpret_cast<char*>(&vocabRows), sizeof(int));
		out.write(reinterpret_cast<char*>(&vocabCols), sizeof(int));
		out.write(reinterpret_cast<char*>(&vocabType), sizeof(int));

		Mat vocabCont = vocabulary.isContinuous() ? vocabulary : vocabulary.clone();
		out.write(reinterpret_cast<const char*>(vocabCont.data), vocabRows * vocabCols * vocabCont.elemSize());
	}
	else {
		int zero = 0;
		out.write(reinterpret_cast<char*>(&zero), sizeof(int)); // rows = 0
		out.write(reinterpret_cast<char*>(&zero), sizeof(int)); // cols = 0
		out.write(reinterpret_cast<char*>(&zero), sizeof(int)); // type = 0
	}

	// 4. Save descriptors (one per image ID)
	int featureCount = features.size();
	out.write(reinterpret_cast<char*>(&featureCount), sizeof(int));
	for (const auto& [imageId, f] : features) {
		int idLen = imageId.size();
		out.write(reinterpret_cast<const char*>(&idLen), sizeof(int));
		out.write(imageId.c_str(), idLen);

		const Mat& desc = f->getDescriptor();
		if (desc.empty()) continue;

		CV_Assert(desc.rows == 1 && desc.type() == CV_32F);

		int rows = desc.rows;
		int cols = desc.cols;
		int type = desc.type();

		out.write(reinterpret_cast<char*>(&rows), sizeof(int));
		out.write(reinterpret_cast<char*>(&cols), sizeof(int));
		out.write(reinterpret_cast<char*>(&type), sizeof(int));
		out.write(reinterpret_cast<const char*>(desc.data), rows * cols * sizeof(float));
	}

	out.close();
	log.writeToFeatureDatabaseLog("Index saved to: " + indexFile);
	return true;
}

bool Indexer::createFolderIfNotExists(string folderPath) {
	try {
		// Check if the folder already exists
		if (!fs::exists(folderPath)) {
			// Attempt to create the folder and any intermediate directories
			if (fs::create_directories(folderPath)) {
				cout << "Folder created successfully: " << folderPath << endl;
			}
			else {
				cout << "Failed to create folder: " << folderPath << endl;
				return false;
			}
		}
		else {
			// Folder already exists
			cout << "Folder already exists: " << folderPath << endl;
		}
		return true;
	}
	catch (const fs::filesystem_error& e) {
		// Catch any exceptions thrown by filesystem operations
		cout << "Filesystem error: " << e.what() << endl;
		return false;
	}
}

bool Indexer::readIndex(string indexPath) {
	string selectedFeature = utils.extractFileName(indexPath);
	cout << selectedFeature << endl;

	indexPath += "/index.bin";
	cout << "Reading index from: " << indexPath << endl;

	ifstream in(indexPath, ios::binary);
	if (!in) {
		cerr << "Failed to open file for reading: " << indexPath << endl;
		return false;
	}

	features.clear();  // Now a map<string, Feature*>
	vocabulary.release();

	// Step 0: Read vocabulary
	int vocabRows = 0, vocabCols = 0, vocabType = 0;
	in.read(reinterpret_cast<char*>(&vocabRows), sizeof(int));
	in.read(reinterpret_cast<char*>(&vocabCols), sizeof(int));
	in.read(reinterpret_cast<char*>(&vocabType), sizeof(int));

	if (vocabRows > 0 && vocabCols > 0) {
		vocabulary.create(vocabRows, vocabCols, vocabType);
		size_t vocabSize = vocabRows * vocabCols * vocabulary.elemSize();
		in.read(reinterpret_cast<char*>(vocabulary.data), vocabSize);
		cout << "Vocabulary loaded. Size: " << vocabRows << "x" << vocabCols << endl;
	}
	else {
		cout << "No vocabulary found in index." << endl;
	}

	// Step 1: Read features
	int featureCount = 0;
	in.read(reinterpret_cast<char*>(&featureCount), sizeof(int));

	for (int i = 0; i < featureCount; ++i) {
		// Read image ID (string)
		int idLen = 0;
		in.read(reinterpret_cast<char*>(&idLen), sizeof(int));
		string imageId(idLen, '\0');
		in.read(&imageId[0], idLen);

		// Read descriptor
		int rows = 0, cols = 0, type = 0;
		in.read(reinterpret_cast<char*>(&rows), sizeof(int));
		in.read(reinterpret_cast<char*>(&cols), sizeof(int));
		in.read(reinterpret_cast<char*>(&type), sizeof(int));

		Mat descriptor(rows, cols, type);
		size_t dataSize = rows * cols * CV_ELEM_SIZE(type);
		in.read(reinterpret_cast<char*>(descriptor.data), dataSize);

		// Instantiate appropriate feature class
		Feature* f = nullptr;
		if (selectedFeature == "Color Histogram")
			f = new ColorHistogram();
		else if (selectedFeature == "HOG")
			f = new HOG();
		else if (selectedFeature == "Color Correlogram")
			f = new ColorCorrelogram();
		else if (selectedFeature == "SIFT")
			f = new SIFTFeature();
		else if (selectedFeature == "ORB")
			f = new ORBFeature();

		if (!f) continue;

		f->setDescriptor(descriptor);
		f->setId(imageId);
		features[imageId] = f;
	}

	in.close();
	return true;
}


map<string, Feature*> Indexer::getFeatures() {
	return features;
}

Mat Indexer::getVocab() {
	return vocabulary;
}