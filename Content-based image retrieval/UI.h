#pragma once

#include <windows.h>
#include <commdlg.h>  // cần cho OPENFILENAMEA
#include <shlobj.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <string>
#include <iostream>
#include "Image.h"
#include "ImageDatabase.h"
#include "Indexer.h"
#include "Query.h"
#include "Logs.h"
#include "Time.h"
#include "Evaluate.h"

using namespace std;
using namespace cv;

enum UIMode { FEATURE_EXTRACTION, QUERY_MODE };

class ImageRetrievalUI {
private:
    ImageDatabase imagedatabase;
    Indexer indexer;
    Query query;
    Log log;

    Evaluator evaluator;
    Rect mAPBox;

    Timer timer;
    double featureExtractionExecutiontime;
    double queryExecutionTime;
    Rect featureExtractTimeBox;
    Rect queryTimeBox;

    bool evaluateMode = false;
    bool queriable;
    bool dropdownOpen = false;

    Image originalImage;
    vector<pair<Mat, float>> retrievedImages;
    vector<Image> queryImages;
    string windowName = "Content-based Image Retrieval";

    int scrollOffset = 0; // Thêm vào trong class
    int dropdownScrollOffset = 0;  // thêm biến offset cho dropdown
    const int maxDropdownVisibleItems = 3; // tối đa số dòng hiển thị dropdown

    string imagePathInput;
    string imageDatabasePathInput;
    string featureDatabasePathOutput;

    vector<string> featureMethods = { "Color Histogram", "Color Correlogram", "SIFT", "HOG", "ORB"};
    int selectedMethodIndex = -1;

    // Các vùng giao diện
    Rect browseBox = Rect(260, 450, 90, 20);
    Rect loadButton = Rect(100, 500, 200, 50);
    Rect retrieveButton = Rect(470, 500, 200, 50);
    Rect dropdownBox;
    bool loadActive = false;
    string selectedFeature;


    Rect evaluateButton = Rect(600, 450, 60, 20);

    Rect kTopBox;
    bool inputActive = false;
    string kTopText = "0";  // Default value
    string kTopBuffer;


    Rect inputPathBox = Rect(50, 10, 200, 40);
    Rect browseInputBox = Rect(100, 0, 90, 20);
    Rect outputPathBox = Rect(50, 50, 200, 40);
    Rect browseOutputBox = Rect(100, 20, 90, 20);

    Rect upArrowRect, downArrowRect;

    UIMode currentMode = FEATURE_EXTRACTION; // Default mode

    // New GUI elements for Feature Extraction tab
    Rect extractButton;
    Rect featureInputBox, featureBrowseBox;
    string featureInputPath;

    // GUI elements for Query tab (already exist)
    Rect queryButton;
    Rect queryInputBox, queryBrowseBox;
    Rect queryIndexBox, queryIndexBrowseBox;
    string queryInputPath, queryIndexPath;

    // Tab selector
    Rect featureTabButton, queryTabButton;

    // Mouse Callback
    static void onMouse(int event, int x, int y, int, void* userdata);

    void extractFeatureAndIndexing();
    void drawRetrievedImages(Mat& ui);
    void drawUI();
    string browseDataFolder(string);
    string browseImageFile();
    string browseFolder(string);
    
    void queryImage();

    void changeToEvaluateMode();


    string extractFileName(const string&);

public:
    ImageRetrievalUI() {
        namedWindow(windowName, WINDOW_NORMAL);
        setMouseCallback(windowName, onMouse, this);
    }
    string getClipboardText();

    Mat getImg();
    void run();
};
