#include "UI.h"
#include "Tester.h"

int main(int argc, char* argv[]) {
	if (argc < 3) {
		// Run main application
		ImageRetrievalUI app;
		app.run();
	}
	else if (argc == 3) {
		Tester tester(argv[1], argv[2]);
		tester.runTestFeatureExtraction();
		tester.writeExtractionResultToFile(argv[1], argv[2], "Extraction_result");
	}
	else if (argc == 4) {
		Tester tester(argv[1], argv[2], argv[3]);
		tester.runTestQuery();
		tester.writeQueryResultToFile(argv[1], argv[2], atoi(argv[3]), "Query_result");
	}
	else {
		return 1;
	}

	return 0;
}