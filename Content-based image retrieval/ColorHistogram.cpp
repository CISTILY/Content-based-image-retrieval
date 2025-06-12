#include "ColorHistogram.h"

void ColorHistogram::createFeature(String image_id, Mat src_image) {
    vector<Mat> bgr_planes;
    split(src_image, bgr_planes);
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;
    Mat blue_hist, green_hist, red_hist;
    calcHist(&bgr_planes[0], 1, 0, Mat(), blue_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), green_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), red_hist, 1, &histSize, histRange, uniform, accumulate);

    id = image_id;
    r_hist = red_hist.t();
    g_hist = green_hist.t();
    b_hist = blue_hist.t();

    // Concatenate horizontally: result will be 1 x (256*3) = 1 x 768
    Mat concatenated_hist;
    hconcat(vector<Mat>{b_hist, g_hist, r_hist}, concatenated_hist);

    // Store the concatenated histogram in a member Mat (e.g. globalFeatureMat)
    imageDescriptors = concatenated_hist;
}
    
void ColorHistogram::showFeature(String id) {
    int histSize = 256;
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            Scalar(0, 0, 255), 2, 8, 0);
    }
    imshow(id, histImage);
}