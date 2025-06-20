#include "HOG.h"

void HOG::createFeature(String image_id, Mat src_image) {
    src_image.convertTo(src_image, CV_32F, 1 / 255.0);

    // Get gradient image using Sobel
    Mat gx, gy;
    Sobel(src_image, gx, CV_32F, 1, 0, 1);
    Sobel(src_image, gy, CV_32F, 0, 1, 1);

    // Calculate magnitude and angle
    Mat mag, angle;
    cartToPolar(gx, gy, mag, angle, true);  // angle in degrees

    // Compute histogram of oriented gradients
    Mat hogHistogram = computeHOG(mag, angle, true);

    id = image_id;
    setDescriptor(hogHistogram);
}

Mat HOG::computeHOG(InputArray mag, InputArray ang, bool isWeighted)
{
    Mat magMat = mag.getMat();
    Mat angMat = ang.getMat();

    if (magMat.rows != angMat.rows || magMat.cols != angMat.cols) {
        cerr << "Magnitude and angle matrices must have the same dimensions." << endl;
        return Mat();
    }

    int rows = magMat.rows;
    int cols = magMat.cols;

    const int featureDim = 360;
    const float circleDegree = 360.0f;
    const float binLength = circleDegree / featureDim;
    const float halfBin = binLength / 2.0f;

    Mat featureVec = Mat::zeros(1, featureDim, CV_32F);
    vector<float> uplimits(featureDim);
    vector<float> medbins(featureDim);

    for (int i = 0; i < featureDim; ++i) {
        uplimits[i] = (2 * i + 1) * halfBin;
        medbins[i] = i * binLength;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float angleVal = angMat.at<float>(i, j);
            float magnitudeVal = magMat.at<float>(i, j);

            int valueIdx = 0;
            int sideIdx = 0;
            float dif = 0.0f;

            if (angleVal <= uplimits[0] || angleVal >= uplimits[featureDim - 1]) {
                if (!isWeighted) {
                    featureVec.at<float>(0, 0) += magnitudeVal;
                    continue;
                }
                else {
                    if (angleVal >= medbins[0] && angleVal <= uplimits[0]) {
                        dif = abs(angleVal - medbins[0]);
                        valueIdx = 0;
                        sideIdx = 1;
                    }
                    else {
                        dif = abs(angleVal - circleDegree);
                        valueIdx = 0;
                        sideIdx = featureDim - 1;
                    }
                }
            }
            else {
                for (int k = 0; k < featureDim - 1; ++k) {
                    if (angleVal >= uplimits[k] && angleVal < uplimits[k + 1]) {
                        if (!isWeighted) {
                            featureVec.at<float>(0, k + 1) += magnitudeVal;
                            break;
                        }
                        else {
                            dif = abs(angleVal - medbins[k + 1]);
                            valueIdx = k + 1;
                            sideIdx = (angleVal >= medbins[k + 1]) ?
                                ((k + 1 == featureDim - 1) ? 0 : k + 2) : k;
                            break;
                        }
                    }
                }
            }

            if (isWeighted) {
                float prop = (binLength - dif) / binLength;
                float valueToAdd = prop * magnitudeVal;
                float sideValueToAdd = (1.0f - prop) * magnitudeVal;
                featureVec.at<float>(0, valueIdx) += valueToAdd;
                featureVec.at<float>(0, sideIdx) += sideValueToAdd;
            }
        }
    }

    return featureVec;
}