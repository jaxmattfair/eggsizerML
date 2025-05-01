#ifndef MEASUREIMAGE_HPP
#define MEASUREIMAGE_HPP

#include "resultStructs.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <string>

std::vector<eggMeasurement> measureImage(cv::Mat *src, cv::Mat *blobDst,
                                         cv::Mat *polyDst);

#endif
