#include "../include/cannyDetect.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>  // for std::nth_element

double computeMedian(cv::Mat channel) {
    std::vector<uchar> pixels;
    pixels.assign(channel.datastart, channel.dataend);

    size_t n = pixels.size() / 2;
    std::nth_element(pixels.begin(), pixels.begin() + n, pixels.end());
    return pixels[n];
}

void autoCanny(cv::Mat src, cv::Mat *dst, float sigma) {
    // Convert to grayscale if necessary
    cv::Mat gray;
    if (src.channels() == 3) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src.clone();
    }

    // Apply Gaussian Blur to reduce noise
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), sigma);

    // Compute median of the blurred image
    double v = computeMedian(blurred);

    // Calculate threshold values
    double lower = std::max(0.0, (1.0 - sigma) * v);
    double upper = std::min(255.0, (1.0 + sigma) * v);

    // Apply Canny edge detection
    cv::Canny(blurred, *dst, lower, upper);
}
