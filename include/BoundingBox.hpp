#pragma once
#include <opencv2/opencv.hpp>

enum BOX_MODE {DRAWN, DETECTED, HIDDEN};

class BoundingBox
{
public:

    BoundingBox(const cv::Point2i p1, const cv::Point2i p2, BOX_MODE m=DRAWN); //1st Constructor. Often called when user draws by mouse
    BoundingBox(const cv::Rect rectangle, BOX_MODE m=DETECTED); //2nd Constructor. Often called to make boxes from OpenCV output (e.g. detections from a cascade classifier)
    void setTopLeftCorner(const cv::Point2i p);
    void setBottomRightCorner(const cv::Point2i p);
    cv::Rect getRect(void) const;
    bool doesContain(const cv::Point2i p) const; //Returns true if p is in (or on) the box, otherwise returns false
    void move(int deltaX, int deltaY);
    void drawOn(cv::Mat &frame) const;

    cv::Point2i tl; //Top-left
    cv::Point2i tm; //Top-middle
    cv::Point2i tr; //Top-right
    cv::Point2i ml; //Middle-left
    cv::Point2i mr; //Middle-right
    cv::Point2i bl; //Bottom-left
    cv::Point2i bm; //Bottom-middle
    cv::Point2i br; //Bottom-right
    BOX_MODE mode; //DRAWN | DETECTED | HIDDEN

private:

    int width;
    int height;
    void calcAnchros(void); //Calculates anchor positions. It requires Top-left corner, width, and height of the box
};
