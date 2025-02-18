#include "../include/cannyDetect.h"

// < ---------------------------------------- >
// OpenCV Analysis
void autoCanny(cv::Mat src, cv::Mat *dst, float sigma) {
    std::vector<uchar> array;
    // cv::Mat srcCopy;
    // src->copyTo(srcCopy);
    array.assign(src.data, src.data + src.total()*src.channels());
    std::nth_element(array.begin(), array.begin() + 1, array.end(), std::greater{});
    double v = array[1];

    double lower = std::max(0.0, (1.0 - sigma) * v);
    double upper = std::min(255.0, (1.0 + sigma) * v);
    cv::Canny(src, *dst, lower, upper);
}
// < ---------------------------------------- >
