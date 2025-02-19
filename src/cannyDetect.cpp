#include "../include/cannyDetect.h"

// < ---------------------------------------- >
// OpenCV Analysis
void autoCanny(cv::Mat src, cv::Mat *dst, float sigma) {
    cv::Mat blurSrc;
    cv::GaussianBlur(src, blurSrc, cv::Size(7, 7), 0);
    std::vector<uchar> array;
    array.assign(blurSrc.data, blurSrc.data + blurSrc.total()*blurSrc.channels());
    std::nth_element(array.begin(), array.begin() + 1, array.end(), std::greater{});
    double v = array[1];

    double lower = std::max(0.0, (1.0 - sigma) * v);
    double upper = std::min(255.0, (1.0 + sigma) * v);
    cv::Canny(blurSrc, *dst, lower, upper);
}
// < ---------------------------------------- >
