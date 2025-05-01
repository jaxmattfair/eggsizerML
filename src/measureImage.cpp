#include "../include/measureImage.hpp"
#include "../include/blobDetect.h"
#include "../include/measureEdges.h"
#include "../include/otsuThreshold.h"
#include <opencv2/opencv.hpp>

std::vector<eggMeasurement> measureImage(cv::Mat *src, cv::Mat *blobDst,
                                         cv::Mat *polyDst) {
  std::vector<eggMeasurement> results;

  if (src->empty()) {
    return results;
  }

  std::vector<double> blobResults = detectBlobs(src, blobDst);
  cv::Mat prePolyDst;
  otsuThreshold(src, &prePolyDst);
  std::vector<double> otsuResults =
      polyApproxFromEdges(&prePolyDst, polyDst, src);

  int longer_areas = std::max(otsuResults.size(), blobResults.size());

  for (int i = 0; i < longer_areas; i++) {
    eggMeasurement egg;
    egg.eggLabel = i + 1; // Store the image name as egg label
    egg.otsuArea =
        i < otsuResults.size() ? otsuResults[i] : 0.0; // Handle empty areas
    egg.blobArea =
        i < blobResults.size() ? blobResults[i] : 0.0; // Handle empty blobs
    egg.computeWidths();        // Compute widths from areas
    egg.computeAvgAreaPerEgg(); // Compute average area
    results.push_back(egg);
  }

  return results;
}
