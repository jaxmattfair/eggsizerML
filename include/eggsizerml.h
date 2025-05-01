#ifndef EGGSIZERML_H
#define EGGSIZERML_H

// std lib inclusions

// Qt Native Stuff
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardPaths>

// OpenCV2 Inclusions
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Custom Structs
#include "resultStructs.h"

QT_BEGIN_NAMESPACE namespace Ui { class eggsizerML; }
QT_END_NAMESPACE

class eggsizerML : public QMainWindow {
    Q_OBJECT

public:
    eggsizerML(QWidget *parent = nullptr);
    ~eggsizerML();
    struct eggMeasurement {
        int eggLabel;
        double otsuArea;
        double blobArea;
        double avgArea;         // Optional: can also be calculated on-the-fly
        double confidenceScore; // Optional: can also be calculated on-the-fly
        double otsuWidth;
        double blobWidth;

        // Compute width from area (assuming circle: area = π * (d/2)^2)
        static double computeWidthFromArea(double area) {
            if (area <= 0)
                return 0.0;
            return 2.0 * std::sqrt(area / M_PI);
        }

        // compute both widths from areas
        void computeWidths() {
            otsuWidth = computeWidthFromArea(otsuArea);
            blobWidth = computeWidthFromArea(blobArea);
        }

        // Compute average area from both areas
        void computeAvgArea() {
            if (otsuArea < 0 && blobArea < 0) {
                avgArea = 0.0; // No valid areas
            } else if (otsuArea < 0) {
                avgArea = blobArea; // Only blob area is valid
            } else if (blobArea < 0) {
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

private slots:
    // < ---------------------------------------- >
    // BASIC IMAGE UPLOAD/PROCESSING WITH OPENCV
    void open();
    void on_fileOpen_btn_clicked();
    bool loadFile(const QString &fileName);
    void displayResultsTable(const eggResults &results, const std::string &imageName);
    void showNextImage();
    void showPreviousImage();
    void outputResult(eggResults results, const QString &filename,
                      int outputFormat, int emitImage);
    void on_saveResults_btn_clicked();
    void on_folderOpen_btn_clicked();
    void openFolder();
    void onInfoIconHovered();
    // < ---------------------------------------- >

private:
    Ui::eggsizerML *ui;
    QStringList imageFiles;    // Store selected image paths
    int currentImageIndex = 0; // Track which image is displayed

    void loadImageAtIndex(int index);
    void handleEggSegmentation(cv::Mat* src, cv::Mat* dst, cv::Mat* orig, float pixToMM);
};
#endif // EGGSIZERML_H
