#define BOOST_TEST_MODULE EdgeDetectionTest
#include <boost/test/included/unit_test.hpp>
#include <opencv2/opencv.hpp>
#include "edge_detection.hpp"  // Replace with your actual header file

BOOST_AUTO_TEST_CASE(EdgeDetectionAccuracy) {
    // NOTE FOR THIS: WE DO NEED TO ACTUALLY ADD EXAMPLE FILES FOR THE TESTING
    cv::Mat testImage = cv::imread("tests/data/sample_egg.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat expectedEdges = cv::imread("tests/data/sample_egg_edges.jpg", cv::IMREAD_GRAYSCALE);

    // Assume detectEdges is your edge detection function
    cv::Mat detectedEdges = detectEdges(testImage, 100.0, 200.0);

    double similarity = cv::norm(expectedEdges, detectedEdges, cv::NORM_L2) / (testImage.rows * testImage.cols);

    BOOST_CHECK(similarity < 0.1);  // Passes if the detected edges are close to expected
}
