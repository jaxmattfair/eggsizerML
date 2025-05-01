#include "../include/measureEdges.h"
#include <iostream> // Include for debugging

std::vector<double> polyApproxFromEdges(cv::Mat *src, cv::Mat *dst,
                                        cv::Mat *orig, float pixToMM) {
  *dst = orig->clone();
  std::vector<double> egg_areas;
  std::vector<double> egg_widths;

  // contour pre-processed image
  std::vector<std::vector<cv::Point>> contours;
  findContours(*src, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

  // draw contours onto image
  for (int i = 0; i < contours.size(); i++) {
    // very roughly approximately connect contours to form a closed shape
    std::vector<cv::Point> approx;
    approxPolyDP(contours[i], approx, 0.005 * arcLength(contours[i], true),
                 true);

    // if the contour is a closed shape
    double contour_area = contourArea(contours[i]);
    if (contour_area >= 18000 && contour_area <= 700000) {
      // draw the contour
      drawContours(*dst, std::vector<std::vector<cv::Point>>{approx}, 0,
                   cv::Scalar(0, 150, 0), 2);

      // Calculate the area of the poly-dp approximated contour
      double areaMM2 = contour_area / (pixToMM * pixToMM);
      egg_areas.push_back(areaMM2);
    }
  }

  return egg_areas;
}
