#ifndef CANNYDETECT_H
#define CANNYDETECT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void autoCanny(cv::Mat src, cv::Mat *dst, float sigma=0.33);

#endif // CANNYDETECT_H
