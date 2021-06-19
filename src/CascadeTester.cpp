#include <iostream>
#include <numeric>
#include <chrono>

#include "CascadeTester.hpp"
#include "utils.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CascadeTester::CascadeTester(void)
{
    const auto videoName = selectFile("Select test video", {".mp4",".avi"});
    _cap.open(videoName);

    const auto cascadeName = selectFile("Select cascade model file", {".xml"});
    _detector.load(cascadeName);

    _testFolder = selectFolder("Select test folder");

    _TP = 0;
    _FP = 0;
    _FN = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTester::run(void)
{
    const auto testAnnotFiles = listFiles(_testFolder, {".txt"});

    int duration = 0;

    for (const auto& testAnnotFile : testAnnotFiles)
    {
        std::vector<cv::Rect> groundTruthBoxes;

        const auto status = parseGroundTruth(testAnnotFile, groundTruthBoxes);

        if (status)
        {
            const auto frameIdx = extractFrameNumberFromFileName(testAnnotFile);
            _cap.set(cv::CAP_PROP_POS_FRAMES, frameIdx);

            cv::Mat frame;
            _cap.read(frame);

            auto t1 = std::chrono::high_resolution_clock::now();

            std::vector<cv::Rect> detectedObjects;
            _detector.detectMultiScale(frame, detectedObjects);

            auto t2 = std::chrono::high_resolution_clock::now();
            duration += std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

            for (const auto& gt : groundTruthBoxes)
                cv::rectangle(frame, gt, cv::Scalar(0,255,0), 3); //GroundTruth are shown in Green

            for (const auto& obj : detectedObjects)
                cv::rectangle(frame, obj, cv::Scalar(255,0,0), 3); //Cascade detection are shown in Blue

            check(detectedObjects, groundTruthBoxes);

            cv::imshow("Detections", frame);
            cv::waitKey(500);
        }
    }

    std::cout << "Average runtime: " << 1.f*duration/testAnnotFiles.size() << " ms" << std::endl;
    std::cout << "Precision: " << 1.f*_TP/(_TP+_FP) << std::endl;
    std::cout << "Recall: " << 1.f*_TP/(_TP+_FN) << std::endl;

    _cap.release();
    cv::destroyAllWindows();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CascadeTester::parseGroundTruth(const std::string &fileName, std::vector<cv::Rect> &objects) const
{
    const auto name = _testFolder+"/"+fileName;
    std::ifstream gtFile(name);

    int x,y,width,height;

    if (gtFile.is_open())
    {
        while (gtFile >> x >> y >> width >> height)
            objects.push_back(cv::Rect(x,y,width,height));

        gtFile.close();
        return true;
    }
    else
        return false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
uint32_t CascadeTester::extractFrameNumberFromFileName(const std::string& filename) const
{
    size_t idx = filename.find_last_of(".");
    std::string rawname = filename.substr(0, idx);
    return (uint32_t)std::stoi(rawname);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTester::check(const std::vector<cv::Rect> &detectObjects, const std::vector<cv::Rect> &groundTruth)
{
    const auto gtSize = groundTruth.size();
    const auto objSize = detectObjects.size();

    std::vector<int> objMatched(objSize,0);

    for (size_t i=0; i<gtSize; i++)
    {
        for (size_t j=0; j<objSize; j++)
        {
            if (objMatched[j]==0 && IoU(groundTruth[i],detectObjects[j])>0.6)
            {
                objMatched[j]=1;
                break;
            }
        }
    }

    int truePositives = std::accumulate(objMatched.begin(), objMatched.end(), 0);

    _TP += truePositives;
    _FP += objSize-truePositives;
    _FN += gtSize-truePositives;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float CascadeTester::IoU(const cv::Rect& a, const cv::Rect& b) const
{
    float Intersection = (a&b).area();
    float Union = (a|b).area();
    return Intersection/Union;
}
