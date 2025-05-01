#ifndef RESULTSTRUCTS_H
#define RESULTSTRUCTS_H

#include <map>

struct eggMeasurement {
  int eggLabel;
  double otsuArea;
  double blobArea;
  double avgArea;
  double confidenceScore;
  double otsuWidth;
  double blobWidth;
  double avgWidth;

  // Compute width from area (assuming circle: area = π * (d/2)^2)
  double computeWidthFromArea(double area) {
    if (area <= 0)
      return 0.0;
    return 2.0 * std::sqrt(area / M_PI);
  }

  // compute both widths from areas
  void computeWidths() {
    otsuWidth = computeWidthFromArea(otsuArea);
    blobWidth = computeWidthFromArea(blobArea);
  }

  void computeAvgWidthPerEgg() {
    if (otsuWidth <= 0 && blobWidth <= 0) {
      avgWidth = 0.0; // No valid widths
    } else if (otsuWidth <= 0) {
      avgWidth = blobWidth; // Only blob width is valid
    } else if (blobWidth <= 0) {
      avgWidth = otsuWidth; // Only otsu width is valid
    } else {
      avgWidth = (otsuWidth + blobWidth) / 2.0; // Average of both widths
    }
  }

  // Compute average area from both areas
  void computeAvgAreaPerEgg() {
    if (otsuArea <= 0 && blobArea <= 0) {
      avgArea = 0.0; // No valid areas
    } else if (otsuArea <= 0) {
      avgArea = blobArea; // Only blob area is valid
    } else if (blobArea <= 0) {
      avgArea = otsuArea; // Only otsu area is valid
    } else {
      avgArea = (otsuArea + blobArea) / 2.0; // Average of both areas
    }
  }

  // Compute confidence score based on mean & std. dev of eggs in image
  void computeConfidence() { confidenceScore = 0.5; }
};

struct eggResults {
  // Each entry is: <imageName, list of that image's egg measurements>
  std::map<std::string, std::vector<eggMeasurement>> imageMeasurements;

  // Compute confidence score for each egg in each image
  // (Just calls computeConfidence() on each eggMeasurement)
  void computeConfidence() {
    for (auto &pair : imageMeasurements) {
      for (eggMeasurement &egg : pair.second) {
        egg.computeConfidence();
      }
    }
  }
};
#endif // RESULTSTRUCTS_H
