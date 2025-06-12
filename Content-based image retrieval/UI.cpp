#include"UI.h"
string ImageRetrievalUI::extractFileName(const string& filePath) {
    size_t lastSlash = filePath.find_last_of("\\/");
    if (lastSlash == string::npos) return filePath;
    return filePath.substr(lastSlash + 1);
}

void ImageRetrievalUI::onMouse(int event, int x, int y, int, void* userdata) {
    ImageRetrievalUI* self = reinterpret_cast<ImageRetrievalUI*>(userdata);
    if (event == EVENT_LBUTTONDOWN) {
        Point pt(x, y);

        // Handle tab switching
        if (self->featureTabButton.contains(pt)) {
            self->currentMode = FEATURE_EXTRACTION;
            return;
        }
        if (self->queryTabButton.contains(pt)) {
            self->currentMode = QUERY_MODE;
            return;
        }

        // Handle file browsing based on mode
        if (self->currentMode == FEATURE_EXTRACTION) {
            if (self->featureBrowseBox.contains(pt)) {
                self->featureInputPath = self->browseFolder("Select an image database");
            }
            else if (self->extractButton.contains(pt)) {
                self->extractFeatureAndIndexing();
            }
        }
        else if (self->currentMode == QUERY_MODE) {
            if (self->queryBrowseBox.contains(pt)) {
                self->queryInputPath = self->browseImageFile();
            }
            else if (self->queryIndexBrowseBox.contains(pt)) {
                self->queryIndexPath = self->browseDataFolder("Select feature database");
                self->selectedFeature = self->extractFileName(self->queryIndexPath);
                self->loadActive = true;
            }
            else if (self->queryButton.contains(pt)) {
                self->queryImage();
            }
            else if (self->upArrowRect.contains(Point(x, y))) {
                if (self->scrollOffset > 0) {
                    self->scrollOffset--;
                }
            }
            else if (self->downArrowRect.contains(Point(x, y))) {
                int totalRows = (self->retrievedImages.size() + 1) / 2;
                int maxRows = 300 / (250 + 50);
                if ((self->scrollOffset + maxRows) < totalRows) {
                    self->scrollOffset++;
                }
            }
            else if (self->kTopBox.contains(Point(x, y))) {
                self->inputActive = true;
                self->kTopBuffer = "";  // Reset buffer for new input
            }
            else {
                self->inputActive = false;
            }
        }

        // Xử lý dropdown chọn phương pháp
        if (self->dropdownBox.contains(pt)) {
            self->dropdownOpen = !self->dropdownOpen; // bật/tắt danh sách
            return;
        }
        // Nếu dropdown mở
        else if (self->dropdownOpen) {
            // Kiểm tra nếu click vào vùng item
            for (int i = 0; i < self->featureMethods.size(); ++i) {
                int visibleIndex = i - self->dropdownScrollOffset;
                if (visibleIndex < 0 || visibleIndex >= self->maxDropdownVisibleItems) continue;

                Rect itemRect(self->dropdownBox.x,
                    self->dropdownBox.y + (visibleIndex + 1) * self->dropdownBox.height,
                    self->dropdownBox.width,
                    self->dropdownBox.height);

                if (itemRect.contains(pt)) {
                    self->selectedMethodIndex = i;
                    self->dropdownOpen = false;
                    return;
                }
            }

            // Click ngoài dropdown => đóng lại
            if (!self->dropdownBox.contains(pt)) {
                self->dropdownOpen = false;
            }

            return; // Ngăn xử lý nút khác
        }
    }
}

void ImageRetrievalUI::changeToEvaluateMode() {
    evaluateMode = true;
}

void ImageRetrievalUI::queryImage() {
    map<int, vector<string>> index = indexer.getIndex();
    map<int, vector<float>> centroids = indexer.getCentroid();
    map<int, vector<Feature*>> features = indexer.getFeatures();
    Mat vocabulary = indexer.getVocab();

    cout << "Getting started" << endl;
    cout << index.size() << endl;
    cout << centroids.size() << endl;
    cout << features.size() << endl;

    timer.start();
    query.Search(originalImage.getId(), originalImage.getImg(), index, centroids, features, vocabulary, stoi(kTopText), loadActive ? selectedFeature : featureMethods[selectedMethodIndex]);
    vector<pair<string, float>> results = query.getResult();

    for (int i = 0; i < results.size(); ++i) {
        retrievedImages.push_back(make_pair(imagedatabase.loadImageWithPath(results[i].first), results[i].second));
    }
    evaluator.calculateAveragePrecision(results, originalImage.getId());
    evaluator.calculateMeanAveragePrecision();
    timer.stop();
    queryExecutionTime = timer.elapsedSeconds();
}

void ImageRetrievalUI::selfLoadImage() {
    if (!imagePathInput.empty()) {
        // Find last backslash (or slash for Linux/Mac)
        size_t lastSlash = imagePathInput.find_last_of("\\/");
        string filename = imagePathInput.substr(lastSlash + 1); // "01.jpg"

        // Find last dot to remove extension
        size_t lastDot = filename.find_last_of('.');
        string nameWithoutExt = filename.substr(0, lastDot); // "01"

        originalImage.assignImg(nameWithoutExt, imread(imagePathInput));
        retrievedImages.clear(); // Clear previous results
        cout << "Load ảnh thành công: " << imagePathInput << endl;
    }
    else {
        cout << "Không thể mở file: " << imagePathInput << endl;
    }
}

void ImageRetrievalUI::extractFeatureAndIndexing() {
    timer.start();
    if (!featureInputPath.empty()) {
        imagedatabase.readImageDatabase(featureInputPath, log);
        indexer.indexingImageDatabase(featureInputPath, featureMethods[selectedMethodIndex], imagedatabase, log);
        queriable = true;
    }
    timer.stop();
    featureExtractionExecutiontime = timer.elapsedSeconds();
}

void ImageRetrievalUI::drawRetrievedImages(Mat& ui) {
    const int thumbW = 250, thumbH = 250;
    const int gapX = 70, gapY = 50;
    const int cols = 2;
    const int startX = 530, startY = 320;
    const int visibleHeight = 300; // Display window height

    int maxRows = visibleHeight / (thumbH + gapY);
    int maxVisible = maxRows * cols;

    int scrollStart = scrollOffset * cols;
    int endIdx = min((int)retrievedImages.size(), scrollStart + maxVisible);

    for (int i = scrollStart; i < endIdx; ++i) {
        int localIdx = i - scrollStart;
        int row = localIdx / cols;
        int col = localIdx % cols;

        if (retrievedImages[i].first.empty()) {
            std::cerr << "Warning: retrievedImages[" << i << "] is empty!" << std::endl;
            continue;
        }

        Mat thumb;
        resize(retrievedImages[i].first, thumb, Size(thumbW, thumbH));

        // Calculate aligned position
        int x = startX + col * (thumbW + gapX);
        int y = startY + row * (thumbH + gapY);

        // Only draw inside UI boundary
        if (x + thumbW <= ui.cols && y + thumbH <= ui.rows) {
            thumb.copyTo(ui(Rect(x, y, thumbW, thumbH)));

            // === Draw rank text centered below the image ===
            string rankText = "Rank #" + to_string(i + 1);
            int textBaseLine = 0;
            Size textSize = getTextSize(rankText, FONT_HERSHEY_SIMPLEX, 0.6, 1, &textBaseLine);
            int rankTextX = x + (thumbW - textSize.width) / 2;
            int rankTextY = y + thumbH + textSize.height + 10;

            putText(ui, rankText, Point(rankTextX, rankTextY), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 1);


            string DistanceText = "Distance: " + to_string(retrievedImages[i].second);
            int distanceBaseLine = 0;
            textSize = getTextSize(DistanceText, FONT_HERSHEY_SIMPLEX, 0.6, 1, &distanceBaseLine);
            int distanceTextX = rankTextX - 50;
            int distanceTextY = rankTextY + 30;

            putText(ui, DistanceText, Point(distanceTextX, distanceTextY), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 1);
        }
    }

    // Arrow logic
    int totalRows = (retrievedImages.size() + cols - 1) / cols;
    int arrowX = startX + cols * (thumbW + gapX) - 40;
    int arrowWidth = 30;
    int arrowHeight = 20;

    upArrowRect = Rect(arrowX, startY - arrowHeight - 15, arrowWidth, arrowHeight);
    downArrowRect = Rect(arrowX, startY + (maxRows - 1) * (thumbH + gapY) + thumbH + 25, arrowWidth, arrowHeight);

    // === Arrow Drawing ===
    if (scrollOffset > 0) {
        // Up arrow
        Point ptsUp[3] = {
            Point(upArrowRect.x, upArrowRect.y + upArrowRect.height),
            Point(upArrowRect.x + arrowWidth / 2, upArrowRect.y),
            Point(upArrowRect.x + arrowWidth, upArrowRect.y + upArrowRect.height)
        };
        fillConvexPoly(ui, ptsUp, 3, Scalar(100, 100, 100));
    }

    if ((scrollOffset + maxRows) < totalRows) {
        // Down arrow
        Point ptsDown[3] = {
            Point(downArrowRect.x, downArrowRect.y),
            Point(downArrowRect.x + arrowWidth / 2, downArrowRect.y + arrowHeight),
            Point(downArrowRect.x + arrowWidth, downArrowRect.y)
        };
        fillConvexPoly(ui, ptsDown, 3, Scalar(100, 100, 100));
    }
}

string ImageRetrievalUI::browseDataFolder(string title) {
    BROWSEINFOA bi;
    CHAR folderPath[MAX_PATH] = { 0 };

    ZeroMemory(&bi, sizeof(bi));
    bi.lpszTitle = title.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = NULL;
    bi.lParam = 0;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

    if (pidl != 0) {
        // Get folder path
        SHGetPathFromIDListA(pidl, folderPath);
        string selectedFolder(folderPath);

        if (!indexer.readIndex(selectedFolder)) {
            cout << "Can not load index file" << endl;
        }

        // Example usage:
        cout << "Selected folder: " << selectedFolder << endl;

        // Giải phóng bộ nhớ
        CoTaskMemFree(pidl);

        return selectedFolder;
    }
    else {
        cout << "No folder selected" << endl;
        return "";
    }
}

string ImageRetrievalUI::browseImageFile() {
    char filename[MAX_PATH] = { 0 };

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp\0All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = "Select an image";

    if (GetOpenFileNameA(&ofn)) {
        // Convert to string
        string strPath(filename);

        // Find last backslash (or slash for Linux/Mac)
        size_t lastSlash = strPath.find_last_of("\\/");
        string filename = strPath.substr(lastSlash + 1); // "01.jpg"

        // Find last dot to remove extension
        size_t lastDot = filename.find_last_of('.');
        string nameWithoutExt = filename.substr(0, lastDot); // "01"

        originalImage.assignImg(nameWithoutExt, imread(strPath));
        retrievedImages.clear(); // Clear previous results
        cout << "Load ảnh thành công: " << strPath << endl;

        return strPath;
    }
    else {
        return "";
    }
}

string ImageRetrievalUI::browseFolder(string title) {
    BROWSEINFOA bi;
    CHAR folderPath[MAX_PATH] = { 0 };

    ZeroMemory(&bi, sizeof(bi));
    bi.lpszTitle = title.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = NULL;
    bi.lParam = 0;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

    if (pidl != 0) {
        // Get folder path
        SHGetPathFromIDListA(pidl, folderPath);
        string selectedFolder(folderPath);

        // Example usage:
        cout << "Selected folder: " << selectedFolder << endl;

        // Xóa dữ liệu ảnh cũ nếu cần
        retrievedImages.clear();

        // Giải phóng bộ nhớ
        CoTaskMemFree(pidl);

        return selectedFolder;
    }
    else {
        cout << "No folder selected" << endl;
        return "";
    }
}

void ImageRetrievalUI::drawUI() {
    // Screen
    Mat ui(720, 1280, CV_8UC3, Scalar(240, 240, 240));

    // Tittle
    putText(ui, "Image Retrieval System", Point(460, 40),
        FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

    // Draw tab buttons 
    featureTabButton = Rect(50, 60, 200, 25);
    queryTabButton = Rect(250, 60, 200, 25);
    Rect frame = Rect(50, 85, 1180, 585);
    rectangle(ui, frame, Scalar(0, 0, 0, 1), 1);
    rectangle(ui, featureTabButton, currentMode == FEATURE_EXTRACTION ? Scalar(180, 180, 255) : Scalar(200, 200, 200), -1);
    rectangle(ui, queryTabButton, currentMode == QUERY_MODE ? Scalar(180, 180, 255) : Scalar(200, 200, 200), -1);
    putText(ui, "Feature Extraction", featureTabButton.tl() + Point(30, 17), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2);
    putText(ui, "Query", queryTabButton.tl() + Point(75, 17), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2);

    // Feature extraction tab
    if (currentMode == FEATURE_EXTRACTION) {

        // Second frame
        Rect frame2 = Rect(80, 220, 1120, 425);
        rectangle(ui, frame2, Scalar(0, 0, 0, 1), 1);

        // Input box 1
        putText(ui, "Choose an image dataset", featureInputBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        featureInputBox = Rect(80, 120, 990, 20);
        featureBrowseBox = Rect(1100, 120, 100, 20);
        rectangle(ui, featureInputBox, Scalar(255, 255, 255), -1);
        rectangle(ui, featureInputBox, Scalar(0, 0, 0), 1);
        putText(ui, featureInputPath, featureInputBox.tl() + Point(5, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        rectangle(ui, featureBrowseBox, Scalar(100, 100, 200), -1);
        putText(ui, "Browse", featureBrowseBox.tl() + Point(27, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);

        if (!log.getImageDatabaseLog().empty()) {
            putText(ui, "Image database parsing phase started", frame2.tl() + Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
            for (int i = 2; i <= log.getImageDatabaseLog().size() + 1; ++i) {
                putText(ui, log.getImageDatabaseLog()[i-2], frame2.tl() + Point(10, i * 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
            }
            putText(ui, "Feature database extracting phase started", frame2.tl() + Point(10, (log.getImageDatabaseLog().size() + 2) * 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
            for (int i = 2; i <= log.getFeatureDatabaseLog().size() + 1; ++i) {
                putText(ui, log.getFeatureDatabaseLog()[i - 2], frame2.tl() + Point(10, (log.getImageDatabaseLog().size() + i + 1) * 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
            }
        }

        // Extract button
        extractButton = Rect(1100, 170, 100, 20);
        rectangle(ui, extractButton, Scalar(100, 100, 200), -1);
        putText(ui, "Extract", extractButton.tl() + Point(27, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);

        // Draw execution time box for feature extraction selection
        putText(ui, "Execution time", featureExtractTimeBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        featureExtractTimeBox = Rect(600, 170, 170, 20); // Vị trí dropdown
        rectangle(ui, featureExtractTimeBox, Scalar(255, 255, 255), -1);
        rectangle(ui, featureExtractTimeBox, Scalar(0, 0, 0), 1);
        putText(ui, to_string(featureExtractionExecutiontime) + " seconds", Point(featureExtractTimeBox.x + 5, featureExtractTimeBox.y + 13),
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

        // Draw dropdown box for feature extraction selection
        putText(ui, "Feature extraction method", dropdownBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        dropdownBox = Rect(800, 170, 270, 20); // Vị trí dropdown
        rectangle(ui, dropdownBox, Scalar(255, 255, 255), -1);
        rectangle(ui, dropdownBox, Scalar(0, 0, 0), 1);
        if (selectedMethodIndex == -1) {
            putText(ui, "Choose a feature extraction method", Point(dropdownBox.x + 5, dropdownBox.y + 13),
                FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        }
        else {
            putText(ui, featureMethods[selectedMethodIndex], Point(dropdownBox.x + 5, dropdownBox.y + 13),
                FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        }

        rectangle(ui, dropdownBox, Scalar(100, 100, 100), 1); // viền

        // Mũi tên chỉ xuống
        line(ui, Point(dropdownBox.br().x - 20, dropdownBox.y + 5),
            Point(dropdownBox.br().x - 15, dropdownBox.y + 15), Scalar(0, 0, 0), 1);
        line(ui, Point(dropdownBox.br().x - 15, dropdownBox.y + 15),
            Point(dropdownBox.br().x - 10, dropdownBox.y + 5), Scalar(0, 0, 0), 1);

        // Nếu dropdown đang mở, vẽ danh sách
        if (dropdownOpen) {
            int totalItems = featureMethods.size();
            int visibleItems = min(maxDropdownVisibleItems, totalItems);

            for (int i = 0; i < visibleItems; ++i) {
                int methodIdx = i + dropdownScrollOffset;
                if (methodIdx >= totalItems) break;

                Rect itemRect(dropdownBox.x,
                    dropdownBox.y + (i + 1) * dropdownBox.height,
                    dropdownBox.width - 10,
                    dropdownBox.height);

                rectangle(ui, itemRect, Scalar(220, 220, 220), -1);
                putText(ui, featureMethods[methodIdx], Point(itemRect.x + 5, itemRect.y + 13),
                    FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
                rectangle(ui, itemRect, Scalar(150, 150, 150), 1);
            }

            // Thanh cuộn bên phải
            if (totalItems > maxDropdownVisibleItems) {
                int barX = dropdownBox.x + dropdownBox.width - 10;
                int barY = dropdownBox.y + dropdownBox.height;
                int barH = dropdownBox.height * visibleItems;

                rectangle(ui, Rect(barX, barY, 10, barH), Scalar(200, 200, 200), -1);

                float ratio = float(visibleItems) / totalItems;
                int scrollH = max(20, int(barH * ratio));
                int scrollY = barY + int(dropdownScrollOffset * (barH - scrollH) / float(totalItems - visibleItems));

                rectangle(ui, Rect(barX, scrollY, 10, scrollH), Scalar(100, 100, 100), -1);
            }
        }
    }
    else if (currentMode == QUERY_MODE) {

        // Second frame
        Rect frame2 = Rect(80, 270, 1120, 375);
        rectangle(ui, frame2, Scalar(0, 0, 0, 1), 1);

        // Index selection box
        putText(ui, "Choose an index", queryIndexBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        queryIndexBox = Rect(80, 170, 990, 20);
        queryIndexBrowseBox = Rect(1100, 170, 100, 20);
        rectangle(ui, queryIndexBox, Scalar(255, 255, 255), -1);
        rectangle(ui, queryIndexBox, Scalar(0, 0, 0), 1);
        putText(ui, queryIndexPath, queryIndexBox.tl() + Point(5, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        rectangle(ui, queryIndexBrowseBox, Scalar(100, 100, 200), -1);
        putText(ui, "Browse", queryIndexBrowseBox.tl() + Point(27, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);

        // Input image box
        putText(ui, "Choose an image", queryInputBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        queryInputBox = Rect(80, 120, 990, 20);
        queryBrowseBox = Rect(1100, 120, 100, 20);
        rectangle(ui, queryInputBox, Scalar(255, 255, 255), -1);
        rectangle(ui, queryInputBox, Scalar(0, 0, 0), 1);
        putText(ui, queryInputPath, queryInputBox.tl() + Point(5, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        rectangle(ui, queryBrowseBox, Scalar(100, 100, 200), -1);
        putText(ui, "Browse", queryBrowseBox.tl() + Point(27, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);

        // Query button
        queryButton = Rect(1100, 220, 100, 20);
        rectangle(ui, queryButton, Scalar(100, 100, 200), -1);
        putText(ui, "Query", queryButton.tl() + Point(27, 13), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);

        // Draw execution time box for querying
        putText(ui, "Execution time", queryTimeBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        queryTimeBox = Rect(400, 220, 170, 20); // Vị trí dropdown
        rectangle(ui, queryTimeBox, Scalar(255, 255, 255), -1);
        rectangle(ui, queryTimeBox, Scalar(0, 0, 0), 1);
        putText(ui, to_string(queryExecutionTime) + " seconds", Point(queryTimeBox.x + 5, queryTimeBox.y + 13),
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

        // Draw mAP box for querying
        putText(ui, "mAP", mAPBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        mAPBox = Rect(200, 220, 170, 20); // Vị trí dropdown
        rectangle(ui, mAPBox, Scalar(255, 255, 255), -1);
        rectangle(ui, mAPBox, Scalar(0, 0, 0), 1);
        putText(ui, to_string(evaluator.getMAP()), Point(mAPBox.x + 5, mAPBox.y + 13),
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

        // Draw kTop box for querying
        putText(ui, "kTop", kTopBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        kTopBox = Rect(600, 220, 170, 20); // Vị trí dropdown
        rectangle(ui, kTopBox, Scalar(255, 255, 255), -1);
        rectangle(ui, kTopBox, Scalar(0, 0, 0), 1);
        string toShow = inputActive ? kTopBuffer : kTopText;
        putText(ui, toShow, Point(kTopBox.x + 5, kTopBox.y + 13),
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

        // Draw dropdown box for querying
        putText(ui, "Feature extraction method", dropdownBox.tl() - Point(0, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
        dropdownBox = Rect(800, 220, 270, 20); // Vị trí dropdown
        rectangle(ui, dropdownBox, Scalar(255, 255, 255), -1);
        rectangle(ui, dropdownBox, Scalar(0, 0, 0), 1);
        if (selectedMethodIndex == -1) {
            putText(ui, loadActive ? selectedFeature : "Choose a feature extraction method", Point(dropdownBox.x + 5, dropdownBox.y + 13),
                FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        }
        else {
            putText(ui, featureMethods[selectedMethodIndex], Point(dropdownBox.x + 5, dropdownBox.y + 13),
                FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
        }

        rectangle(ui, dropdownBox, Scalar(100, 100, 100), 1); // viền

        // Mũi tên chỉ xuống
        line(ui, Point(dropdownBox.br().x - 20, dropdownBox.y + 5),
            Point(dropdownBox.br().x - 15, dropdownBox.y + 15), Scalar(0, 0, 0), 1);
        line(ui, Point(dropdownBox.br().x - 15, dropdownBox.y + 15),
            Point(dropdownBox.br().x - 10, dropdownBox.y + 5), Scalar(0, 0, 0), 1);

        // Nếu dropdown đang mở, vẽ danh sách
        if (dropdownOpen) {
            int totalItems = featureMethods.size();
            int visibleItems = min(maxDropdownVisibleItems, totalItems);

            for (int i = 0; i < visibleItems; ++i) {
                int methodIdx = i + dropdownScrollOffset;
                if (methodIdx >= totalItems) break;

                Rect itemRect(dropdownBox.x,
                    dropdownBox.y + (i + 1) * dropdownBox.height,
                    dropdownBox.width - 10,
                    dropdownBox.height);

                rectangle(ui, itemRect, Scalar(220, 220, 220), -1);
                putText(ui, featureMethods[methodIdx], Point(itemRect.x + 5, itemRect.y + 13),
                    FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
                rectangle(ui, itemRect, Scalar(150, 150, 150), 1);
            }

            // Thanh cuộn bên phải
            if (totalItems > maxDropdownVisibleItems) {
                int barX = dropdownBox.x + dropdownBox.width - 10;
                int barY = dropdownBox.y + dropdownBox.height;
                int barH = dropdownBox.height * visibleItems;

                rectangle(ui, Rect(barX, barY, 10, barH), Scalar(200, 200, 200), -1);

                float ratio = float(visibleItems) / totalItems;
                int scrollH = max(20, int(barH * ratio));
                int scrollY = barY + int(dropdownScrollOffset * (barH - scrollH) / float(totalItems - visibleItems));

                rectangle(ui, Rect(barX, scrollY, 10, scrollH), Scalar(100, 100, 100), -1);
            }
        }
        Rect originalBox = Rect(130, 320, 250, 250);
        // Khung ảnh gốc
        rectangle(ui, originalBox, Scalar(180, 180, 180), 2);
        putText(ui, "Original Image", Point(180, 300),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2);

        putText(ui, "Retrieved Images", Point(720, 300),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2);

        // Hiển thị ảnh gốc nếu có
        if (!originalImage.getImg().empty()) {
            Mat resized;
            resize(originalImage.getImg(), resized, Size(250, 250));
            resized.copyTo(ui(Rect(130, 320, 250, 250)));
        }

        // Hiển thị các ảnh truy xuất (tối đa 3)
        drawRetrievedImages(ui);

        // Nút "Evaluate"
        /*rectangle(ui, evaluateButton.tl(), evaluateButton.br(), Scalar(100, 0, 100), -1);
        putText(ui, "Evaluate mode", Point(evaluateButton.x + 20, evaluateButton.y + 15),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);*/

    }
    imshow(windowName, ui);
}

void ImageRetrievalUI::run() {
    while (true) {
        drawUI();

        int key = waitKey(20);

        if (key == 27) break;  // ESC thoát

        if (dropdownOpen) {
            if (key == 'w' || key == 'W') {
                if (dropdownScrollOffset > 0) dropdownScrollOffset--;
            }
            else if (key == 's' || key == 'S') {
                int maxOffset = featureMethods.size() - maxDropdownVisibleItems;
                if (dropdownScrollOffset < maxOffset) dropdownScrollOffset++;
            }
        }

        if (inputActive) {
            if (key >= '0' && key <= '9') {
                kTopBuffer += (char)key;
            }
            else if (key == 8 && !kTopBuffer.empty()) {
                kTopBuffer.pop_back(); // Handle backspace
            }
            else if ((key == 13 || key == 10) && !kTopBuffer.empty()) {
                kTopText = kTopBuffer; // Save entered value
                inputActive = false;
            }
        }
    }
}

Mat ImageRetrievalUI::getImg() {
    return originalImage.getImg();
}