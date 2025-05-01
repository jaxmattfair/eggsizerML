#include "../include/blobDetect.h"

std::vector<double> detectBlobs(const cv::Mat &src, cv::Mat &dst, float pixToMM) {
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

    // Draw detected blobs and labels
    dst = src.clone();
    cv::drawKeypoints(src, keypoints, dst, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // Return blob areas and annotate with numbers
    std::vector<double> blob_areas;
    for (size_t i = 0; i < keypoints.size(); ++i) {
        const auto &kp = keypoints[i];
        blob_areas.push_back((kp.size * kp.size * 3.14) / (4 * pixToMM));

        // Draw index number at blob center
        cv::Point center(static_cast<int>(kp.pt.x), static_cast<int>(kp.pt.y));
        cv::putText(dst, std::to_string(i + 1), center,
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 2);
    }

    return blob_areas;
}
