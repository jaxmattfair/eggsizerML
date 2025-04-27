#define BOOST_TEST_MODULE MeasurementAccuracyTest
#include <boost/test/included/unit_test.hpp>
#include "../include/measurement.hpp"
#include "../include/cannyDetect.h"
#include "../include/measureEdges.h"
#include "../include/blobDetect.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

void runMeasurementAccuracyTests(
    const std::string& species,
    const std::string& dataFilePath,
    const std::string& imageFolderPath,
    double allowedPercentError = 5.0
    )
{
    std::ifstream csv(dataFilePath);
    std::string line;

    BOOST_REQUIRE_MESSAGE(csv.is_open(), "CSV file could not be opened: " + dataFilePath);

    while (std::getline(csv, line)) {
        std::stringstream ss(line);
        std::string speciesName, imageId, widthStr;
        std::getline(ss, speciesName, ',');
        std::getline(ss, imageId, ',');
        std::getline(ss, widthStr, ',');

        if (speciesName != species) continue;

        if (widthStr.empty()) {
            BOOST_FAIL("Empty width string for image: " + imageId);
        }

        double expectedWidth = std::stod(widthStr);
        std::string imagePath = imageFolderPath + "/" + imageId + ".jpg";

        BOOST_TEST_MESSAGE("Testing image: " + imagePath);
        BOOST_REQUIRE_MESSAGE(fs::exists(imagePath), "Image file not found: " + imagePath);

        // ---- Blob detection width measurement ----
        MeasurementResult blobResult = measureEdges(imagePath);

        // ---- Updated edge detection (autoCanny + polyApproxFromEdges) ----
        cv::Mat orig = cv::imread(imagePath);
        BOOST_REQUIRE_MESSAGE(!orig.empty(), "Failed to load image: " + imagePath);

        cv::Mat cannyDst;
        autoCanny(&orig, &cannyDst);

        cv::Mat polyDst;
        EdgeMeasureResults edgeResults = polyApproxFromEdges(&cannyDst, &polyDst, &orig);

        // ---- Calculating average widths ----
        double measuredBlobWidth = 0.0;
        if (!blobResult.widths.empty()) {
            measuredBlobWidth = std::accumulate(blobResult.widths.begin(), blobResult.widths.end(), 0.0) / blobResult.widths.size();
        }

        double measuredEdgeWidth = 0.0;
        if (!edgeResults.widths.empty()) {
            measuredEdgeWidth = std::accumulate(edgeResults.widths.begin(), edgeResults.widths.end(), 0.0) / edgeResults.widths.size();
        }

        // ---- Error checking ----
        double blobErrorPercent = 100.0 * std::abs(measuredBlobWidth - expectedWidth) / expectedWidth;
        double edgeErrorPercent = 100.0 * std::abs(measuredEdgeWidth - expectedWidth) / expectedWidth;

        BOOST_CHECK_MESSAGE(
            blobErrorPercent <= allowedPercentError,
            "Blob measurement for " + imageId + " is off by " + std::to_string(blobErrorPercent) + "%. Expected: " + std::to_string(expectedWidth) + ", got: " + std::to_string(measuredBlobWidth)
            );

        BOOST_CHECK_MESSAGE(
            edgeErrorPercent <= allowedPercentError,
            "Edge-based measurement for " + imageId + " is off by " + std::to_string(edgeErrorPercent) + "%. Expected: " + std::to_string(expectedWidth) + ", got: " + std::to_string(measuredEdgeWidth)
            );
    }
}

BOOST_AUTO_TEST_CASE(Cisco_Egg_Measurements) {
    fs::path testPath = fs::path(__FILE__);
    fs::path root = testPath.parent_path().parent_path();
    std::cout << "cisco";
    runMeasurementAccuracyTests(
        "cisco",
        "../tests/data/data.csv",
        "../tests/data/cisco",
        5.0 // Allow 5% error
        );
}

BOOST_AUTO_TEST_CASE(Walleye_Egg_Measurements) {
    fs::path testPath = fs::path(__FILE__);
    fs::path root = testPath.parent_path().parent_path();
    std::cout << "walleye";
    runMeasurementAccuracyTests(
        "walleye",
        "../tests/data/data.csv",
        "../tests/data/samples",
        5.0
    );
}

BOOST_AUTO_TEST_CASE(Lake_Trout_Egg_Measurements) {
    fs::path testPath = fs::path(__FILE__);
    fs::path root = testPath.parent_path().parent_path();
    std::cout << "lake trout";
    runMeasurementAccuracyTests(
        "lake trout",
        "../tests/data/data.csv",
        "../tests/data/LT_eggs",
        5.0
    );
}
