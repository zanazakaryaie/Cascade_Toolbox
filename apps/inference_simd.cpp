#include <opencv2/opencv.hpp>
#include <chrono>
#define SIMD_OPENCV_ENABLE
#include <Simd/SimdDetection.hpp>
#include "utils.hpp"

using namespace cv;
using namespace std;

int main()
{
    string videoName = selectFile("Select Video File", {"*.mp4", "*.avi"});
    VideoCapture cap(videoName);

    string modelName = selectFile("Select Cascade Model", {"*.xml"});

    typedef Simd::Detection<Simd::Allocator> Detection;

    Detection detection;
    detection.Load(modelName);

    bool inited = false;

    while (cap.isOpened())
    {
        Mat frame;
        cap.read(frame);

        if (frame.empty())
            break;

        Detection::View image = frame;

        auto t1 = std::chrono::high_resolution_clock::now();

        if (!inited)
        {
            detection.Init(image.Size(), 1.1);
            inited = true;
        }

        Detection::Objects objects;
        detection.Detect(image, objects);

        auto t2 = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

        for (const auto& object:objects)
            rectangle(frame, object.rect, Scalar(0,255,0), 3);

        ostringstream FPS;
        FPS << "FPS: " << 1000/duration;
        putText(frame, FPS.str(), Point(10,30), 0, 1, Scalar(0,0,255), 2);

        imshow("Detections", frame);

        if (waitKey(1)=='q')
            break;
    }

    cap.release();

    return 0;
}
