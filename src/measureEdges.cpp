#include "../include/measureEdges.h"

std::vector<double> polyApproxFromEdges(cv::Mat* src, cv::Mat* dst, cv::Mat* orig, float pixToMM) {
    // set up output mat + array
    *dst = orig->clone();
    std::vector<double> egg_areas;

    // contour pre-processed image
    std::vector<std::vector<cv::Point>> contours;
    findContours(*src, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // draw contours onto image
    for (int i = 0; i < contours.size(); i++) {
        // very roughly approximately connect contours to form a closed shape
        std::vector<cv::Point> approx;
        approxPolyDP(contours[i], approx, 0.005 * arcLength(contours[i], true), true);

        // if the contour is a closed shape
        if (contourArea(contours[i]) >= 18000 && contourArea(contours[i]) <= 700000) {
            // draw the contour
            drawContours(*dst, std::vector<std::vector<cv::Point>>{approx}, 0, cv::Scalar(0, 150, 0), 2);

            // calculate the area of the poly-dp approximated contour
            egg_areas.push_back(contourArea(approx) / pixToMM);
        }
    }

    return egg_areas;
}
