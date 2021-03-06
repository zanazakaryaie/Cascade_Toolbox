#pragma once

#include <iostream>
#include <numeric>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

class CascadeTester
{
public:

    CascadeTester(void); //Constructor. Asks the use to select the test video file, the trained cascade model file, and the test data folder (generated by collect_test_data)
    void run(void); //Runs the CascadeClassifier on each annotated frame and calculates Precision/Recall

private:

    bool parseGroundTruth(const std::string &fileName, std::vector<cv::Rect> &objects) const; //Parses boxes from the annotation file
    int extractFrameNumberFromFileName(const std::string &filename) const;
    void check(const std::vector<cv::Rect> &detectObjects, const std::vector<cv::Rect> &groundTruth); //Counts and updates TP,FP,and FN
    float IoU(const cv::Rect &a, const cv::Rect &b) const; //Calculates Intersection-over-Union of two boxes

    cv::VideoCapture cap;
    cv::CascadeClassifier detector;
    std::string testFolder;

    int TP; //True Positive
    int FP; //False Positive
    int FN; //False Negative
};
