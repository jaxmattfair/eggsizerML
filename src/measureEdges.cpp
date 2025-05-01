#include "../include/measureEdges.h"

std::vector<double> polyApproxFromEdges(cv::Mat* src, cv::Mat* dst, cv::Mat* orig, float pixToMM) {
    *dst = orig->clone();
    std::vector<double> egg_areas;

    std::vector<std::vector<cv::Point>> contours;
    findContours(*src, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    int label = 1;
    for (int i = 0; i < contours.size(); i++) {
        std::vector<cv::Point> approx;
        approxPolyDP(contours[i], approx, 0.005 * arcLength(contours[i], true), true);

        double area = contourArea(contours[i]);
        if (area >= 18000 && area <= 700000) {
            drawContours(*dst, std::vector<std::vector<cv::Point>>{approx}, 0, cv::Scalar(0, 150, 0), 2);
            egg_areas.push_back(contourArea(approx) / pixToMM);

            // Compute centroid using image moments
            cv::Moments M = moments(approx);
            if (M.m00 != 0) {
                int cx = static_cast<int>(M.m10 / M.m00);
                int cy = static_cast<int>(M.m01 / M.m00);
                cv::putText(*dst, std::to_string(label), cv::Point(cx, cy),
                            cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 2);
            }

            ++label;
        }
    }

    return egg_areas;
}
