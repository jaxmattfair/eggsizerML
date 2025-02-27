#include "../include/blobDetect.h"

void detectBlobs(const cv::Mat &src, cv::Mat &dst) {
    // Convert to grayscale if necessary
    cv::Mat gray;
    if (src.channels() == 3) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src.clone();
    }

    // Set up SimpleBlobDetector parameters
    cv::SimpleBlobDetector::Params params;
    params.filterByArea = true;
    params.minArea = 5000;
    params.maxArea = 500000;
    params.filterByCircularity = false;
    params.filterByConvexity = false;
    params.filterByInertia = false;

    // Create SimpleBlobDetector
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

    // Detect blobs
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(gray, keypoints);

    // Draw detected blobs
    dst = src.clone();
    cv::drawKeypoints(src, keypoints, dst, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}
