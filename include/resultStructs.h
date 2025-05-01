#ifndef RESULTSTRUCTS_H
#define RESULTSTRUCTS_H

#include <map>

struct eggMeasurement {
  int eggLabel;
  double otsuArea;
  double blobArea;
  double avgArea;
  double otsuConfidence;
  double blobConfidence;
  double confidence;
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
};

struct eggResults {
  // Each entry is: <imageName, list of that image's egg measurements>
  std::map<std::string, std::vector<eggMeasurement>> imageMeasurements;

  // Compute confidence score for each egg in each image
  // (Just calls computeConfidence() on each eggMeasurement)
  void computeConfidence() {
    for (auto &pair : imageMeasurements) {
      for (eggMeasurement &egg : pair.second) {
        if (!(egg.otsuArea >= 0) || !(egg.blobArea >= 0)) {
          egg.otsuConfidence = 0.0; // Invalid areas
          egg.blobConfidence = 0.0; // Invalid areas
        } else {
          egg.otsuConfidence =
              1.0 - abs(egg.otsuArea - egg.avgArea) / egg.avgArea;
          egg.blobConfidence =
              1.0 - abs(egg.blobArea - egg.avgArea) / egg.avgArea;
          // clamp confidence to [0, 1]
          egg.otsuConfidence = std::max(0.0, std::min(1.0, egg.otsuConfidence));
          egg.blobConfidence = std::max(0.0, std::min(1.0, egg.blobConfidence));
          egg.confidence = (egg.otsuConfidence + egg.blobConfidence) / 2.0;
        }
      }
    }
  }
};
#endif // RESULTSTRUCTS_H
