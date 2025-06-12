#include "Indexer.h"
#include "ColorHistogram.h"
#include "ColorCorrelogram.h"
#include "SIFT.h"
#include "ORB.h"
#include "HOG.h"

Indexer::~Indexer() {
	// Iterate over each cluster and its associated feature list
	for (auto& [clusterId, featureList] : clusters) {
		// Delete each Feature* in the list
		for (Feature* feature : featureList) {
			delete feature;
		}
		// Optionally clear the feature list to free up memory immediately
		featureList.clear();
	}

	// Clear the entire clusters map
	clusters.clear();
}

void Indexer::indexingImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase imageDatabase, Log& log) {
	Mat labels, centers;
	vector<Feature*> extractedFeatures;

	// Extract features from all images in the database
	extractFeatureImageDatabase(imageDatabasePath, selectedFeature, imageDatabase, extractedFeatures, log);

	// Cluster extracted features into groups (e.g., using k-means with 5 clusters)
	clustering(extractedFeatures, 5, labels, centers);
	cout << "Cluster done" << endl;

	// Create an index based on the clustering result and save to disk
	saveIndex(imageDatabasePath, selectedFeature, extractedFeatures, log, labels, centers);

	// Log completion of index saving
	log.writeToFeatureDatabaseLog("Save index done");
}

void Indexer::extractFeatureImageDatabase(string imageDatabasePath, string selectedFeature, ImageDatabase database, vector<Feature*>& extractedFeatures, Log& log) {
	const int dictionarySize = 99; // You can parameterize this

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

		// Step 1: Extract raw descriptors
		for (int i = 0; i < database.getImage().size(); i++) {
			Feature* feat = (selectedFeature == "SIFT") ? static_cast<Feature*>(new SIFTFeature) : static_cast<Feature*>(new ORBFeature);
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

bool Indexer::saveIndex(string indexPath, string selectedFeature, vector<Feature*>& extractedFeatures, Log& log, Mat labels, Mat centers) {
	// 1. Cluster features and index them
	for (int i = 0; i < extractedFeatures.size(); ++i) {
		int clusterId = labels.at<int>(i);
		clusters[clusterId].push_back(extractedFeatures[i]);
		index[clusterId].push_back(extractedFeatures[i]->getId());
	}

	// 2. Save centroids
	for (int i = 0; i < centers.rows; ++i) {
		vector<float> centroid(centers.cols);
		memcpy(centroid.data(), centers.ptr<float>(i), centers.cols * sizeof(float));
		centroids[i] = centroid;
	}

	// 3. Prepare output path
	indexPath = extractPath(indexPath) + "extracted_feature/" + selectedFeature;
	createFolderIfNotExists(indexPath);
	string indexFile = indexPath + "/index.bin";

	ofstream out(indexFile, ios::binary | ios::trunc);
	if (!out) {
		cerr << "Failed to open file for writing: " << indexFile << endl;
		return false;
	}

	// 4. Save vocabulary (only for BoVW-type features)
	if (!vocabulary.empty()) {
		int vocabRows = vocabulary.rows;
		int vocabCols = vocabulary.cols;
		int vocabType = vocabulary.type();

		out.write(reinterpret_cast<char*>(&vocabRows), sizeof(int));
		out.write(reinterpret_cast<char*>(&vocabCols), sizeof(int));
		out.write(reinterpret_cast<char*>(&vocabType), sizeof(int));
		out.write(reinterpret_cast<const char*>(vocabulary.data), vocabRows * vocabCols * vocabulary.elemSize());
	}
	else {
		// Mark no vocabulary with 0s
		int zero = 0;
		out.write(reinterpret_cast<char*>(&zero), sizeof(int)); // rows = 0
		out.write(reinterpret_cast<char*>(&zero), sizeof(int)); // cols = 0
		out.write(reinterpret_cast<char*>(&zero), sizeof(int)); // type = 0
	}

	// 5. Save centroids
	int centroidCount = centroids.size();
	int featureDim = centroidCount > 0 ? centroids.begin()->second.size() : 0;
	out.write(reinterpret_cast<char*>(&centroidCount), sizeof(int));
	out.write(reinterpret_cast<char*>(&featureDim), sizeof(int));
	for (const auto& [clusterId, centroid] : centroids) {
		out.write(reinterpret_cast<const char*>(&clusterId), sizeof(int));
		out.write(reinterpret_cast<const char*>(centroid.data()), featureDim * sizeof(float));
	}

	// 6. Save index mapping (image IDs)
	int indexCount = index.size();
	out.write(reinterpret_cast<char*>(&indexCount), sizeof(int));
	for (const auto& [clusterId, imageIds] : index) {
		out.write(reinterpret_cast<const char*>(&clusterId), sizeof(int));
		int idCount = imageIds.size();
		out.write(reinterpret_cast<char*>(&idCount), sizeof(int));
		for (const string& id : imageIds) {
			uint32_t len = static_cast<uint32_t>(id.size());
			out.write(reinterpret_cast<char*>(&len), sizeof(uint32_t));
			out.write(id.c_str(), len);
		}
	}

	// 7. Save descriptors (BoVW or global)
	int clusterCount = clusters.size();
	out.write(reinterpret_cast<char*>(&clusterCount), sizeof(int));
	for (const auto& [clusterId, featureList] : clusters) {
		out.write(reinterpret_cast<const char*>(&clusterId), sizeof(int));
		int featureCount = featureList.size();
		out.write(reinterpret_cast<char*>(&featureCount), sizeof(int));

		for (Feature* f : featureList) {
			const Mat& desc = f->getDescriptor();

			// Enforce: desc is 1xN, CV_32F (for BoVW/global)
			CV_Assert(desc.rows == 1 && desc.type() == CV_32F);

			int rows = desc.rows;
			int cols = desc.cols;
			int type = desc.type();
			out.write(reinterpret_cast<char*>(&rows), sizeof(int));
			out.write(reinterpret_cast<char*>(&cols), sizeof(int));
			out.write(reinterpret_cast<char*>(&type), sizeof(int));
			out.write(reinterpret_cast<const char*>(desc.data), rows * cols * sizeof(float));
		}
	}

	out.close();
	log.writeToFeatureDatabaseLog("Cluster index saved to: " + indexFile);
	return true;
}

void Indexer::clustering(vector<Feature*>& features, int clusterCount, Mat& labels, Mat& centers) {
	if (features.empty()) {
		cerr << "No feature descriptors found.\n";
		return;
	}

	// Prepare the descriptor matrix
	Mat data;

	for (Feature* f : features) {
		const Mat& desc = f->getDescriptor();
		if (desc.empty()) {
			cerr << "Empty descriptor found.\n";
			return;
		}

		Mat desc32F;
		if (desc.type() != CV_32F)
			desc.convertTo(desc32F, CV_32F);
		else
			desc32F = desc;

		if (desc32F.rows > 1) {
			data.push_back(desc32F);  // local descriptors: add each row
		}
		else {
			data.push_back(desc32F.reshape(1, 1));  // global: ensure 1xN
		}
	}

	if (data.empty() || data.rows < clusterCount) {
		cerr << "Not enough data to perform clustering.\n";
		return;
	}

	// Perform k-means clustering
	kmeans(
		data,
		clusterCount,
		labels,
		TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 100, 1.0),
		3,
		KMEANS_PP_CENTERS,
		centers
	);

	cout << "Clustering completed. Cluster count: " << clusterCount << ", Data size: " << data.rows << endl;
}

string Indexer::extractPath(string filePath) {
	size_t lastSlash = filePath.find_last_of("\\/");
	if (lastSlash == string::npos)
		return filePath; // No directory separator found
	return filePath.substr(0, lastSlash + 1); // Include the slash
}

string Indexer::extractFileName(string filePath) {
	size_t lastSlash = filePath.find_last_of("\\/");
	if (lastSlash == string::npos)
		return filePath; // No directory separator found
	return filePath.substr(lastSlash + 1); // Exclude the slash
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
	string selectedFeature = extractFileName(indexPath);
	cout << selectedFeature << endl;

	indexPath += "/index.bin";
	cout << "Reading index from: " << indexPath << endl;

	ifstream in(indexPath, ios::binary);
	if (!in) {
		cerr << "Failed to open file for reading: " << indexPath << endl;
		return false;
	}

	index.clear();
	centroids.clear();
	clusters.clear();
	vocabulary.release();

	// Step 0: Read vocabulary (BoVW only)
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

	// Step 1: Read centroids
	int centroidCount = 0, featureDim = 0;
	in.read(reinterpret_cast<char*>(&centroidCount), sizeof(int));
	in.read(reinterpret_cast<char*>(&featureDim), sizeof(int));

	for (int i = 0; i < centroidCount; ++i) {
		int clusterId = 0;
		in.read(reinterpret_cast<char*>(&clusterId), sizeof(int));

		vector<float> centroid(featureDim);
		in.read(reinterpret_cast<char*>(centroid.data()), featureDim * sizeof(float));

		centroids[clusterId] = centroid;
	}

	// Step 2: Read index
	int indexCount = 0;
	in.read(reinterpret_cast<char*>(&indexCount), sizeof(int));

	for (int i = 0; i < indexCount; ++i) {
		int clusterId = 0, numIds = 0;
		in.read(reinterpret_cast<char*>(&clusterId), sizeof(int));
		in.read(reinterpret_cast<char*>(&numIds), sizeof(int));

		for (int j = 0; j < numIds; ++j) {
			uint32_t idLen = 0;
			in.read(reinterpret_cast<char*>(&idLen), sizeof(uint32_t));

			string imageId(idLen, '\0');
			in.read(&imageId[0], idLen);

			index[clusterId].push_back(imageId);
			cout << "Image ID: " << imageId << " -> Cluster: " << clusterId << endl;
		}
	}

	// Step 3: Read descriptors
	int clusterCount = 0;
	in.read(reinterpret_cast<char*>(&clusterCount), sizeof(int));

	for (int i = 0; i < clusterCount; ++i) {
		int clusterId = 0, featureCount = 0;
		in.read(reinterpret_cast<char*>(&clusterId), sizeof(int));
		in.read(reinterpret_cast<char*>(&featureCount), sizeof(int));

		for (int j = 0; j < featureCount; ++j) {
			int rows = 0, cols = 0, type = 0;
			in.read(reinterpret_cast<char*>(&rows), sizeof(int));
			in.read(reinterpret_cast<char*>(&cols), sizeof(int));
			in.read(reinterpret_cast<char*>(&type), sizeof(int));

			Mat descriptor(rows, cols, type);
			size_t dataSize = rows * cols * CV_ELEM_SIZE(type);
			in.read(reinterpret_cast<char*>(descriptor.data), dataSize);

			// Dynamically assign feature class
			Feature* f = nullptr;
			if (selectedFeature == "Color Histogram")
				f = new ColorHistogram();
			else if (selectedFeature == "HOG")
				f = new HOG();
			else if (selectedFeature == "Color Correlogram")
				f = new ColorCorrelogram();
			else if (selectedFeature == "SIFT")
				f = new SIFTFeature();  // Holds BoVW histogram now
			else if (selectedFeature == "ORB")
				f = new ORBFeature();   // Holds BoVW histogram now

			if (!f) continue;

			f->setDescriptor(descriptor);
			clusters[clusterId].push_back(f);
		}
	}

	in.close();
	return true;
}

map<int, vector<string>> Indexer::getIndex() {
	return index;
}

map<int, vector<float>> Indexer::getCentroid() {
	return centroids;
}

map<int, vector<Feature*>> Indexer::getFeatures() {
	return clusters;
}

Mat Indexer::getVocab() {
	return vocabulary;
}