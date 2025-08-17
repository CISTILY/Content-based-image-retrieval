#pragma once

#include <windows.h>
#include <commdlg.h>  // cần cho OPENFILENAMEA
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
#include "Utils.h"

using namespace std;
using namespace cv;

enum UIMode { FEATURE_EXTRACTION, QUERY_MODE };

class ImageRetrievalUI {
private:
    ImageDatabase imagedatabase;                ///< Object that manages the image dataset
    Indexer indexer;                            ///< Handles feature extraction and indexing
    Query query;                                ///< Executes image search based on descriptors
    Log log;                                    ///< Logging utility
    Utils utils;                                ///< Helper functions
    Evaluator evaluator;                        ///< Evaluator for mAP, AP metrics
	Timer timer;                                ///< Timer for benchmarking operations
	
	bool queriable = false;                     ///< Indicates if the query can be executed

	// Ktop input box
    Rect kTopBox;
    bool inputkTopActive = false;
    string kTopText = "1";  // Default value
    string kTopBuffer;

    // Vocabulary size input
    Rect vocabularySizeBox;
    bool inputVocabActive = false;
    string vocabularySizeText = "100"; // Default value
    string vocabBuffer;

    // Default mode
    UIMode currentMode = FEATURE_EXTRACTION; 

	// GUI elements for dropdown menu
	Rect dropdownBox;                           ///< Rectangle for dropdown menu
	bool loadActive = false;                    ///< Indicates if the dropdown is active
	string selectedFeature;                     ///< Selected feature extraction method
    bool dropdownOpen = false;                  ///< Indicates if the dropdown menu is open
    int scrollOffset = 0;                       ///< Offset for scrolling through images
    int dropdownScrollOffset = 0;               ///< Offset for scrolling through dropdown items
    const int maxDropdownVisibleItems = 3;      ///< Maximum number of visible items in dropdown
	vector<string> featureMethods = { "Color Histogram", "Color Correlogram", "SIFT", "HOG", "ORB" };   ///< List of feature extraction methods
	int selectedMethodIndex = -1;               ///< Index of the selected method in the dropdown

    // GUI elements for Feature Extraction tab
    Rect extractButton;
    Rect featureInputBox, featureBrowseBox;
    string featureInputPath;

    Rect featureExtractTimeBox;                 ///< Rectangle for displaying feature extraction time
    double featureExtractionExecutiontime = 0.0;      ///< Execution time for feature extraction

    // GUI elements for Query tab
    Rect queryButton;
    Rect queryInputBox, queryBrowseBox;
    Rect queryIndexBox, queryIndexBrowseBox;
    string queryInputPath, queryIndexPath;

	// Varables for querying results
	Image originalImage;                        ///< Original query image
	vector<pair<Mat, float>> retrievedImages;   ///< List of retrieved images with distances
	string windowName = "Content-based Image Retrieval";    ///< Name of the UI window
    Rect mAPBox;                                ///< Rectangle for displaying mAP results
	Rect upArrowRect, downArrowRect;            ///< Rectangles for up/down arrows in dropdown menu   
	Rect ResultsBox;                            ///< Rectangle for displaying results
    
    Rect queryTimeBox;                          ///< Rectangle for displaying query execution time
    double queryExecutionTime = 0.0;                  ///< Execution time for query operation

    // Tab selector
	Rect featureTabButton, queryTabButton;      ///< Rectangles for feature extraction and query tabs

    /**
     * @brief Callback for mouse events on the UI window.
     * 
     * @param [in] event        Mouse event type.
     * @param [in] x            X-coordinate of mouse.
     * @param [in] y            Y-coordinate of mouse.
     * @param [in] userdata     Pointer to this UI class.
     * 
     * @return void
     */
    static void onMouse(int event, int x, int y, int, void* userdata);

    /**
     * @brief Executes feature extraction and indexing for the selected image database.
     * 
     * @return void
     */
    void extractFeatureAndIndexing();

    /**
     * @brief Draws retrieved images on the UI with distance and rank info (scrollable).
     * 
     * @param[in,out] ui The OpenCV Mat representing the UI canvas.
     * 
     * @return void
     */
    void drawRetrievedImages(Mat& ui);

    /**
     * @brief Draws all visual elements in the current UI frame.
     * 
     * @return void
     */
    void drawUI();

    /**
     * @brief Opens a folder browsing dialog to select a directory.
     * 
     * @param[in] title Dialog title string.
     * 
     * @return Path to selected folder.
     */
    string browseFolder(string title);

    /**
     * @brief Opens a file dialog to select an image file.
     * 
     * @return Path to selected image file.
     */
    string browseImageFile();

    /**
     * @brief Browse and select a directory for input data.
     * 
     * @param[in] title Dialog title.
     * 
     * @return Folder path selected.
     */
    string browseDataFolder(string title);

    /**
     * @brief Displays retrieved images in grid mode (e.g., 5 per row).
     * 
     * @param[in] datasetName Name of the dataset (for title or filtering).
     * 
	 * @return void
     */
    void drawRetrievedImagesGrid(string datasetName);

    /**
     * @brief Performs the image retrieval process using the selected query image.
     * 
	 * @return void
     */
    void queryImage();

public:
    /**
     * @brief Constructor that sets up the UI window and mouse callback.
     */
    ImageRetrievalUI() {
        namedWindow(windowName, WINDOW_NORMAL);
        setMouseCallback(windowName, onMouse, this);
    }

    /**
     * @brief Returns the current displayed image in the UI.
     * 
     * @return cv::Mat representing the displayed UI image.
     */
    Mat getImg();

    /**
     * @brief Runs the UI loop and listens for interactions.
     * 
	 * @return void
     */
    void run();
};