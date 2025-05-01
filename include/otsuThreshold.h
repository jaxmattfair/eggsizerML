#ifndef CANNYDETECT_H
#define CANNYDETECT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void otsuThreshold(cv::Mat *src, cv::Mat *dst);

#endif // CANNYDETECT_H
