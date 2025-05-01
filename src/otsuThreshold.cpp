#include "../include/otsuThreshold.h"
#include <opencv2/imgproc/imgproc.hpp>

void otsuThreshold(cv::Mat *src, cv::Mat *dst) {
  // Convert to grayscale if necessary
  cv::Mat gray;
  if (src->channels() == 3) {
    cv::cvtColor(*src, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = src->clone();
  }

  // Apply Gaussian Blur to reduce noise
  cv::Mat blurred;
  cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0.33);

  // Apply Otsu's edge detection
  cv::threshold(blurred, *dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
