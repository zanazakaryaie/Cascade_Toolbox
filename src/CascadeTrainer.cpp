#include <fstream>
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "utils.hpp"
#include "CascadeTrainer.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CascadeTrainer::CascadeTrainer(void)
{
    checkPreviousTraining();
    readConfigs();
    generatePosVecFile();
    generateNegTxtFile();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::start(void)
{
    const auto modelFolder = std::filesystem::current_path().string() + "/model/";

    createDirectory(modelFolder);

    runTrainCascadeCommand();

    //Check if all stages have been completed
    const auto stageNames = listFiles(modelFolder, {".xml"});

    int nStagesCompleted = stageNames.size()-1; //excluding params.xml file

    if (nStagesCompleted < _nStages)
    {
        _nStages = nStagesCompleted;
        runTrainCascadeCommand();
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::checkPreviousTraining(void) const
{
    const auto modelFolder = std::filesystem::current_path().string() + "/model/";

    if (doesExist(modelFolder))
    {
        const auto stageNames = listFiles(modelFolder, {".xml"});

        if (!stageNames.empty())
        {
            std::cout << "There seem to be a trained model in " << modelFolder << std::endl;
            std::cout << "Press 'r' to Remove and start training a new model, or press 'c' to continue with this model" << std::endl;

            char input;
            std::cin >> input;

            if (input=='r' || input=='R')
                removeDirectory(modelFolder);
        }
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::readConfigs(void)
{
    const auto configFileName = selectFile("Select config file", {".yaml"});

    cv::FileStorage fs(configFileName, cv::FileStorage::READ);

    if (!fs.isOpened())
        throw std::runtime_error("Error loading config file!");

    _width = fs["width"];
    _height = fs["height"];
    fs["featureType"] >> _featureType;
    _nStages = fs["nStages"];
    _valBufSize = fs["precalcValBufSize"];
    _idxBufSize = fs["precalcIdxBufSize"];
    _posRatio = fs["positiveImageRatio"];

    fs.release();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::generatePosVecFile(void)
{
    const auto positiveFolder = selectFolder("Select Positive Folder");

    const auto posImageNames = listFiles(positiveFolder, {".jpg", ".png", ".bmp"});

    _nPos = posImageNames.size();

    std::ofstream out("positives.txt");

    if (out.is_open())
    {
        const auto resizedPosFolder = positiveFolder+"/Positives_resized/";
        createDirectory(resizedPosFolder);

        for (const auto& pos: posImageNames)
        {
            const auto originalName = positiveFolder+"/"+pos;
            auto img = cv::imread(originalName);
            cv::resize(img, img, cv::Size(_width, _height));

            const auto resizedName = resizedPosFolder + pos;
            cv::imwrite(resizedName, img);

            out << resizedName << " 1 0 0 " << _width-1 << " " << _height-1 << std::endl;
        }

        out.close();

        runCreateSampleCommand();

        remove("positives.txt");
        removeDirectory(resizedPosFolder);
    }
    else
    {
        throw std::runtime_error("Cannot make positives.txt file!");
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::generateNegTxtFile(void)
{
    const auto negativeFolder = selectFolder("Select Negative Folder");

    const auto negImageNames = listFiles(negativeFolder, {".jpg", ".png", ".bmp"}, false);

    _nNegs = negImageNames.size();

    std::ofstream out("negatives.txt");

    if (out.is_open())
    {
        for (const auto& neg: negImageNames)
            out << neg << std::endl;

        out.close();
    }
    else
    {
        throw std::runtime_error("Cannot make negatives.txt file!");
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::runCreateSampleCommand(void) const
{
    std::ostringstream command;
    command << "opencv_createsamples -info positives.txt -num " << _nPos << " -w " << _width << " -h " << _height << " -vec pos-samples.vec" << std::endl;
    int status = system(command.str().c_str());

    if (status!=0)
        throw std::runtime_error("Error in creating .vec file from positive images!");
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::runTrainCascadeCommand(void) const
{
    std::ostringstream command;
    command << "opencv_traincascade -data "<< std::filesystem::current_path().string() << "/model -vec pos-samples.vec -bg negatives.txt -precalcValBufSize "
            <<  _valBufSize << " -precalcIdxBufSize " << _idxBufSize << " -numPos " << _posRatio*_nPos << " -numNeg " << _nNegs << " -numStages "
            << _nStages << " -minHitRate 0.99 -maxFalseAlarmRate 0.5 -w " << _width << " -h " << _height << " -featureType " << _featureType << std::endl;

    int status = system(command.str().c_str());

    if (status!=0)
        throw std::runtime_error("Something went wrong in the training process!");
}
