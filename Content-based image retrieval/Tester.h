#pragma once
#include <string>
#include "UI.h"

class Tester {
private:
	string inputPath;
	string selectedMethod;
	string queryFolderPath;
	string indexPath;
	int kTop;

	float elapsedTimes;
	float queryExecutionTimes;
	float mAP;

	Log log;
	Query query;
	Timer timer;
	Indexer indexer;
	ImageDatabase imagedatabase;
	Evaluator evaluator;

public:
	Tester(char*, char*);
	Tester(char*, char*, char*);

	void writeExtractionResultToFile(char*, char*, string);
	void writeQueryResultToFile(char*, char*, int, string);
	void runTestFeatureExtraction();
	void runTestQuery();
};