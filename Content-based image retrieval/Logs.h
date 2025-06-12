#pragma once

#include <vector>
#include <string>

using namespace std;

class Log {
private:
	vector<string> imageDatabaseLog;
	vector<string> featureDatabaseLog;
public:
	// Constructor
	Log() {}

	// Destructor
	~Log() {}

	// Setters
	void writeToImageDatabaseLog(string);
	void writeToFeatureDatabaseLog(string);

	// Getters
	vector<string> getImageDatabaseLog();
	vector<string> getFeatureDatabaseLog();
};