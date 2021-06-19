#pragma once

#include "BoundingBox.hpp"
#include <opencv2/opencv.hpp>

enum ANNOT_MODE {TRAINING, HARD_MINING, TESTING};

class Annotator
{
public:

    Annotator(ANNOT_MODE m); //Constructor. Opens dialog box for selecting a video file. Creates negative and positive directories.
    void loadCascadeModel(const std::string& modelName); //Optional. If called, the Annotator shows detected objects on each frame. This is used for hard negative mining
    void run(void); //Loads the video and waits for user actions

    ANNOT_MODE mode;

private:

    void selectVideoFile(void); //Opens dialog box to select .mp4 or .avi file
    void selectCascadeModel(void); //Opens dialog box to select .xml file
    void detectObjects(void); //Applies the loaded cascade model (if loaded) to detect objects in the current frame
    static void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata);
    int checkKeyPressed(void); //Returns 1 when space is pressed
    static void checkLeftButtonDown(const cv::Point2i p);
    static void checkMouseMove(const cv::Point2i p);
    static void checkLeftButtonUp(const cv::Point2i p);
    static void checkMiddleButtonDown(const cv::Point2i p);

    static int isAnchor(const cv::Point2i p, int &anchorNo); //Checks whether p is a box anchor or not. If yes, the box number is returned. anchorNo is set to the number of the box anchor (0-7)
    static void editBox(BoundingBox& box, int anchorNo, const cv::Point2i p); //Allows the user to edit the shape of the box by dragging the anchors
    static void drawObjects(void); //Draws all the boxes on frameWithObjects
    static void removeObject(const cv::Point2i p); //Removes the object in which p is pressed inside
    std::vector<cv::Rect> extractRandomNegatives(void); //Picks random negative patches in the current frame
    void saveSamples(void); //Saves negative and positive samples of the current frame
    void saveNegatives(void);
    void saveTestData(void) const;
    void saveTrainData(void);


    cv::VideoCapture _cap;
    cv::CascadeClassifier _detector;
    static bool _drawing;
    static bool _moving;
    static bool _editing;
    static cv::Point2i _p1;
    static std::vector<BoundingBox> _objects; //Positive samples drawn by the user, or detected by the cascade model
    static cv::Mat _frame;
    static cv::Mat _frameWithObjects;
    static int _anchorIdx;
    static int _objectIdx;
    int _frameSteps;
    cv::RNG _rng; //Used for random generation of negative samples
    uint32_t _sumWidth, _sumHeight; //Used to find the average aspect ratio of the positive objects in TRAINING mode. This helps the user to set proper width/height when training a model
};
