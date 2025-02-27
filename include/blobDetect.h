#ifndef BLOBDETECT_H
#define BLOBDETECT_H

#include <opencv2/opencv.hpp>

void detectBlobs(const cv::Mat &src, cv::Mat &dst);

#endif // BLOBDETECT_H
