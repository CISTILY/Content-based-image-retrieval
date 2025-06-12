#include "Logs.h"

void Log::writeToImageDatabaseLog(string message) {
	imageDatabaseLog.push_back(message);
}

vector<string> Log::getImageDatabaseLog() {
	return imageDatabaseLog;
}

void Log::writeToFeatureDatabaseLog(string message) {
	featureDatabaseLog.push_back(message);
}

vector<string> Log::getFeatureDatabaseLog() {
	return featureDatabaseLog;
}