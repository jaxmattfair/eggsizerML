#ifndef BLOBDETECT_H
#define BLOBDETECT_H

#include <opencv2/opencv.hpp>
#include <vector>

struct BlobResults {
    std::vector<double> areas;
    std::vector<double> widths;
};

BlobResults detectBlobs(const cv::Mat &src, cv::Mat &dst, float pixToMM = 100);

#endif // BLOBDETECT_H
