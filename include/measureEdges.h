#ifndef MEASUREEDGES_H
#define MEASUREEDGES_H

// opencv inclusions
#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void polyApproxFromEdges(cv::Mat* src, cv::Mat* dst);

#endif // MEASUREEDGES_H
