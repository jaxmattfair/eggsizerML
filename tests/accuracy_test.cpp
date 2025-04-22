#define BOOST_TEST_MODULE MeasurementAccuracyTest
#include <boost/test/included/unit_test.hpp>
#include "../include/measurement.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

void runMeasurementAccuracyTests(
    const std::string& species,
    const std::string& dataFilePath,
    const std::string& imageFolderPath,
    double allowedPercentError = 5.0
    ) {
    std::ifstream csv(dataFilePath);
    std::string line;

    BOOST_REQUIRE_MESSAGE(csv.is_open(), "CSV file could not be opened: " + dataFilePath);

    while (std::getline(csv, line)) {
        std::stringstream ss(line);
        std::string speciesName, imageId, widthStr;
        std::getline(ss, speciesName, ',');
        std::getline(ss, imageId, ',');
        std::getline(ss, widthStr, ',');  // Assuming the CSV contains the width data instead of diameter

        if (speciesName != species) continue;

        // Check for valid width data
        if (widthStr.empty()) {
            BOOST_FAIL("Empty width string for image: " + imageId);
        } else {
            try {
                double expectedWidth = std::stod(widthStr);  // Using width here, not diameter
            } catch (const std::invalid_argument& e) {
                BOOST_FAIL("Invalid width string: '" + widthStr + "' for image: " + imageId);
            }
        }

        double expectedWidth = std::stod(widthStr);  // Expected width from CSV
        std::string imagePath = imageFolderPath + "/" + imageId + ".jpg";

        BOOST_TEST_MESSAGE("Testing image: " + imagePath);
        BOOST_REQUIRE_MESSAGE(fs::exists(imagePath), "Image file not found: " + imagePath);

        MeasurementResult result = measureEdges(imagePath);

        // Calculate the average width from the blobs (assuming blob_widths contains this data)
        double measuredAvgWidth = 0.0;
        if (!result.widths.empty()) {
            measuredAvgWidth = std::accumulate(result.widths.begin(), result.widths.end(), 0.0) / result.widths.size();
        }

        // Calculate error in percentage
        double errorPercent = 100.0 * std::abs(measuredAvgWidth - expectedWidth) / expectedWidth;

        // output of widths
        if (errorPercent <= allowedPercentError) {
            std::cout << std::to_string(errorPercent);
            std::cout << std::endl;
        }





        BOOST_CHECK_MESSAGE(
            errorPercent <= allowedPercentError,
            "Measurement for " + imageId + " is off by " + std::to_string(errorPercent) + "%. Expected: " + std::to_string(expectedWidth) + ", got: " + std::to_string(measuredAvgWidth)
            );
    }
}

BOOST_AUTO_TEST_CASE(Cisco_Egg_Measurements) {
    fs::path testPath = fs::path(__FILE__);
    fs::path root = testPath.parent_path().parent_path();
    runMeasurementAccuracyTests(
        "cisco",
        "../tests/data/data.csv",
        "../tests/data/cisco",
        5.0 // Allow 5% error
        );
}
