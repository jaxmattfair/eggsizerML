#ifndef MEASUREEDGES_H
#define MEASUREEDGES_H

// opencv inclusions
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

std::vector<double> polyApproxFromEdges(cv::Mat *src, cv::Mat *dst,
                                        cv::Mat *orig, float pixToMM = 100);

#endif // MEASUREEDGES_H
