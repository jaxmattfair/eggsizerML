#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

struct MeasurementResult {
    int numEggs;
    double avgArea;
    double avgWidth;
    double certainty;
    std::vector<double> areas;
    std::vector<double> widths;
};

MeasurementResult measureEdges(const std::string& imagePath);

#endif
