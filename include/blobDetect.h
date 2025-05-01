#ifndef BLOBDETECT_H
#define BLOBDETECT_H

#include <opencv2/opencv.hpp>
#include <vector>

std::vector<double> detectBlobs(const cv::Mat *src, cv::Mat *dst,
                                float pixToMM = 100);

#endif // BLOBDETECT_H
