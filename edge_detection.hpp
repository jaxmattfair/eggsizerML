#ifndef EDGE_DETECTION_HPP
#define EDGE_DETECTION_HPP

#include <opencv2/opencv.hpp>

// Function that takes three arguments: image and two thresholds
cv::Mat detectEdges(const cv::Mat& image, double threshold1, double threshold2);

#endif // EDGE_DETECTION_HPP
