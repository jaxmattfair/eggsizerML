#include "../include/measurement.hpp"
#include "../include/blobDetect.h"
#include <opencv2/opencv.hpp>

MeasurementResult measureEdges(const std::string& imagePath) {
    MeasurementResult result;

    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        result.numEggs = 0;
        result.avgArea = 0.0;
        result.avgWidth = 0.0;
        result.certainty = 0.0;
        return result;
    }

    cv::Mat dst;
    BlobResults blobResults = detectBlobs(image, dst);

    result.areas = blobResults.areas;     // ← Add this
    result.widths = blobResults.widths;   // ← And this

    double totalArea = 0.0;
    for (double a : result.areas) {
        totalArea += a;
    }

    double totalWidth = 0.0;
    for (double width : result.widths) {
        totalWidth += width;
    }

    result.numEggs = result.areas.size();
    result.avgArea = result.areas.empty() ? 0.0 : totalArea / result.areas.size();
    result.avgWidth = result.widths.empty() ? 0.0 : totalWidth / result.widths.size();
    result.certainty = result.areas.empty() ? 0.0 : 0.95;

    return result;
}
