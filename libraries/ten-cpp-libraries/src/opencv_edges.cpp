// opencv_edges.cpp - draw a test image, find its edges with the Canny
// detector and count the shapes with findContours. Pass a file name to save
// the edge image.
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    cv::Mat image(200, 300, CV_8UC1, cv::Scalar(0));                 // black, 8-bit grey
    cv::rectangle(image, {30, 40}, {120, 160}, cv::Scalar(255), cv::FILLED);
    cv::circle(image, {210, 100}, 50, cv::Scalar(180), cv::FILLED);

    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "image " << image.cols << "x" << image.rows
              << ", edge pixels " << cv::countNonZero(edges)
              << ", outer contours " << contours.size() << '\n';
    for (const auto& c : contours) {
        cv::Rect box = cv::boundingRect(c);
        std::cout << "  shape at x=" << box.x << " y=" << box.y
                  << " size " << box.width << "x" << box.height << '\n';
    }
    if (argc > 1)
        std::cout << "saved " << argv[1] << ": " << std::boolalpha
                  << cv::imwrite(argv[1], edges) << '\n';
}
