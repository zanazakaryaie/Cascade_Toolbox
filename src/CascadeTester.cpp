#include "CascadeTester.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CascadeTester::CascadeTester(void)
{
    std::string videoName = selectFile("Select test video", {"*.mp4","*.avi"});
    cap.open(videoName);

    std::string cascadeName = selectFile("Select cascade model file", {"*.xml"});
    detector.load(cascadeName);

    testFolder = selectFolder("Select test folder");

    TP = 0;
    FP = 0;
    FN = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTester::run(void)
{
    std::vector<cv::String> testAnnotFiles = listFiles(testFolder, {"*.txt"});

    int duration = 0;

    for (const auto& testAnnotFile:testAnnotFiles)
    {
        std::vector<cv::Rect> groundTruthBoxes;

        bool status = parseGroundTruth(testAnnotFile, groundTruthBoxes);

        if (status)
        {
            int frameIdx = extractFrameNumberFromFileName(testAnnotFile);
            cap.set(cv::CAP_PROP_POS_FRAMES, frameIdx);

            cv::Mat frame;
            cap.read(frame);

            auto t1 = std::chrono::high_resolution_clock::now();

            std::vector<cv::Rect> detectedObjects;
            detector.detectMultiScale(frame, detectedObjects);

            auto t2 = std::chrono::high_resolution_clock::now();
            duration += std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

            for (const auto& gt:groundTruthBoxes)
                cv::rectangle(frame, gt, cv::Scalar(0,255,0),3); //GroundTruth are shown in Green

            for (const auto& obj:detectedObjects)
                cv::rectangle(frame, obj, cv::Scalar(255,0,0),3); //Cascade detection are shown in Blue

            check(detectedObjects, groundTruthBoxes);

            cv::imshow("Detections", frame);
            cv::waitKey(500);
        }
    }

    std::cout << "Average runtime: " << 1.f*duration/testAnnotFiles.size() << " ms" << std::endl;
    std::cout << "Precision: " << 1.f*TP/(TP+FP) << std::endl;
    std::cout << "Recall: " << 1.f*TP/(TP+FN) << std::endl;

    cap.release();
    cv::destroyAllWindows();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CascadeTester::parseGroundTruth(const std::string &fileName, std::vector<cv::Rect> &objects) const
{
    const std::string name = testFolder+"/"+fileName;
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
int CascadeTester::extractFrameNumberFromFileName(const std::string& filename) const
{
    size_t idx = filename.find_last_of(".");
    std::string rawname = filename.substr(0, idx);
    return stoi(rawname);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTester::check(const std::vector<cv::Rect> &detectObjects, const std::vector<cv::Rect> &groundTruth)
{
    const int gtSize = groundTruth.size();
    const int objSize = detectObjects.size();

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

    TP += truePositives;
    FP += objSize-truePositives;
    FN += gtSize-truePositives;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float CascadeTester::IoU(const cv::Rect& a, const cv::Rect& b) const
{
    float Intersection = (a&b).area();
    float Union = (a|b).area();
    return Intersection/Union;
}
