#ifndef MEASUREEDGES_H
#define MEASUREEDGES_H

// opencv inclusions
#include <opencv2/imgproc/imgproc.hpp>

std::vector<double> polyApproxFromEdges(cv::Mat* thresholded, cv::Mat* orig, cv::Mat* dst, float pixToMM=100);
std::vector<cv::Point> getCentroidsFromEdges(cv::Mat* src, cv::Mat* dst, cv::Mat* orig);

#endif // MEASUREEDGES_H
