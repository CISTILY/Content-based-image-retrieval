#include "ImageDatabase.h"

Mat ImageDatabase::loadImageWithPath(string query_path) {
    return imread(query_path);
}

void ImageDatabase::readImageDatabase(string featureInputPath, Log& log) {
    vector<string> fn_jpg, fn_png, fn;

    // Search for all .jpg files in the directory
    String directory_jpg = featureInputPath + "/*.jpg";
    log.writeToImageDatabaseLog("Opening " + directory_jpg);
    glob(directory_jpg, fn_jpg, false);

    // Search for all .png files in the directory
    String directory_png = featureInputPath + "/*.png";
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

    log.writeToImageDatabaseLog("Parsing folder size...");
    Images.resize(count);  // Pre-allocate space for image objects

    log.writeToImageDatabaseLog("Parsing images...");
    for (size_t i = 0; i < count; i++) {
        Mat image = imread(fn[i]);
        if (image.empty()) {
            // Skip and log if the image failed to load
            log.writeToImageDatabaseLog("Failed to load image: " + fn[i]);
            continue;
        }

        // Assign image data and path to the Image object
        Images[i].assignImg(fn[i], image);
    }

    log.writeToImageDatabaseLog("Parsing done");
}

vector<Image> ImageDatabase::getImage() {
    return Images;
}

void ImageDatabase::showAllImg() {
    for (int i = 0; i < Images.size(); i++) {
        Images[i].showImg();  // Show individual image
    }
    waitKey(0);  // Wait for user input before closing windows
}