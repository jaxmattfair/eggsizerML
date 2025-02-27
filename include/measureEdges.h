#ifndef MEASUREEDGES_H
#define MEASUREEDGES_H

// opencv inclusions
#include <opencv2/imgproc/imgproc.hpp>

void polyApproxFromEdges(cv::Mat* thresholded, cv::Mat* orig, cv::Mat* dst);

#endif // MEASUREEDGES_H
