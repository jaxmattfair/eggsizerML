#include "../include/measureEdges.h"
#include <iostream> // Include for debugging

std::vector<double> polyApproxFromEdges(cv::Mat* src, cv::Mat* dst, cv::Mat* orig, float pixToMM) {
    // Set up output mat + array
    *dst = orig->clone();
    std::vector<double> egg_areas;

    // Contour pre-processed image
    std::vector<std::vector<cv::Point>> contours;
    findContours(*src, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    int eggIndex = 1;  // Start labeling from "Egg 1"

    // Draw contours onto image
    for (int i = 0; i < contours.size(); i++) {
        // Approximate contour to polygon
        std::vector<cv::Point> approx;
        approxPolyDP(contours[i], approx, 0.005 * arcLength(contours[i], true), true);

        // If the contour is a closed shape and within area bounds
        double area = contourArea(approx);
        if (area >= 18000 && area <= 700000) {
            // Draw the contour
            drawContours(*dst, std::vector<std::vector<cv::Point>>{approx}, 0, cv::Scalar(0, 150, 0), 2);

            // Convert area from pixels to mm
            egg_areas.push_back(area / pixToMM);

            // Find centroid of the contour
            cv::Moments m = cv::moments(approx);
            cv::Point centroid(m.m10 / m.m00, m.m01 / m.m00);

            // Label each egg after processing its area
            std::string label = "Egg " + std::to_string(eggIndex);  // Label as "Egg 1", "Egg 2", etc.
            cv::putText(*dst, label, centroid, cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);

            // Increment egg index after labeling
            eggIndex++;
        }
    }
  }

  return egg_areas;
}

std::vector<cv::Point> getCentroidsFromEdges(cv::Mat* src, cv::Mat* dst, cv::Mat* orig) {
    *dst = orig->clone();
    std::vector<cv::Point> egg_centroids;  // Store centroids

    // Contour pre-processed image
    std::vector<std::vector<cv::Point>> contours;
    findContours(*src, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Draw contours onto image
    for (int i = 0; i < contours.size(); i++) {
        // Approximate contour to polygon
        std::vector<cv::Point> approx;
        approxPolyDP(contours[i], approx, 0.005 * arcLength(contours[i], true), true);

        // If the contour is a closed shape
        if (contourArea(contours[i]) >= 18000 && contourArea(contours[i]) <= 700000) {
            // Find centroid of the contour
            cv::Moments m = cv::moments(approx);
            cv::Point centroid(m.m10 / m.m00, m.m01 / m.m00);
            egg_centroids.push_back(centroid);  // Store centroid
        }
    }

    return egg_centroids;  // Return only the centroids
}
