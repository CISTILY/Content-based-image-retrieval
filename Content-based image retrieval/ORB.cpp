#include "ORB.h"

void ORBFeature::createFeature(string image_id, Mat image) {
    id = image_id;

    Mat gray;
    if (image.channels() == 3)
        cvtColor(image, gray, COLOR_BGR2GRAY);
    else
        gray = image.clone();

    Ptr<ORB> orb = ORB::create();
    vector<KeyPoint> keypoints;
    Mat descriptors;

    orb->detectAndCompute(gray, noArray(), keypoints, descriptors);

    if (!descriptors.empty() && descriptors.type() != CV_32F) {
        descriptors.convertTo(descriptors, CV_32F);
    }
    imageDescriptors = descriptors;
}