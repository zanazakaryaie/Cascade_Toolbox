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
    std::string modelFolder = cv::utils::fs::getcwd() + "/model/";

    createDirectory(modelFolder);

    runTrainCascadeCommand();

    //Check if all stages have been completed
    std::vector<cv::String> stageNames = listFiles(modelFolder, {"*.xml"});

    int nStagesCompleted = stageNames.size()-1; //excluding params.xml file

    if (nStagesCompleted < nStages)
    {
        nStages = nStagesCompleted;
        runTrainCascadeCommand();
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::checkPreviousTraining(void) const
{
    std::string modelFolder = cv::utils::fs::getcwd() + "/model/";

    if (doesExist(modelFolder))
    {
        std::vector<cv::String> stageNames = listFiles(modelFolder, {"*.xml"});

        if (stageNames.size()!=0)
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
    std::string configFileName = selectFile("Select config file", {"*.yaml"});

    cv::FileStorage fs(configFileName, cv::FileStorage::READ);

    if (!fs.isOpened())
    {
        std::cerr << "Error loading config file!" << std::endl;
        exit(1);
    }

    width = fs["width"];
    height = fs["height"];
    fs["featureType"] >> featureType;
    nStages = fs["nStages"];
    valBufSize = fs["precalcValBufSize"];
    idxBufSize = fs["precalcIdxBufSize"];
    posRatio = fs["positiveImageRatio"];

    fs.release();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::generatePosVecFile(void)
{
    std::string positiveFolder = selectFolder("Select Positive Folder");

    std::vector<cv::String> posImageNames = listFiles(positiveFolder, {"*.jpg", "*.png", "*.bmp"});

    nPos = posImageNames.size();

    std::ofstream out("positives.txt");

    if (out.is_open())
    {
        std::string resizedPosFolder = positiveFolder+"/Positives_resized/";
        createDirectory(resizedPosFolder);

        for (const auto& pos: posImageNames)
        {
            std::string originalName = positiveFolder+"/"+pos;
            cv::Mat img = cv::imread(originalName);
            cv::resize(img, img, cv::Size(width,height));

            std::string resizedName = resizedPosFolder + pos;
            cv::imwrite(resizedName, img);

            out << resizedName << " 1 0 0 " << width-1 << " " << height-1 <<std::endl;
        }

        out.close();

        runCreateSampleCommand();

        remove("positives.txt");
        removeDirectory(resizedPosFolder);
    }
    else
    {
        std::cerr << "Cannot make positives.txt file!" << std::endl;
        exit(1);
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::generateNegTxtFile(void)
{
    std::string negativeFolder = selectFolder("Select Negative Folder");

    std::vector<cv::String> negImageNames = listFiles(negativeFolder, {"*.jpg", "*.png", "*.bmp"}, false);

    nNegs = negImageNames.size();

    std::ofstream out("negatives.txt");

    if (out.is_open())
    {
        for (const auto& neg: negImageNames)
            out << neg << std::endl;

        out.close();
    }
    else
    {
        std::cerr << "Cannot make negatives.txt file!" << std::endl;
        exit(1);
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::runCreateSampleCommand(void) const
{
    std::ostringstream command;
    command << "opencv_createsamples -info positives.txt -num " << nPos << " -w " << width << " -h " << height << " -vec pos-samples.vec" << std::endl;
    int status = system(command.str().c_str());

    if (status!=0)
    {
        std::cerr << "Error in creating .vec file from positive images!" << std::endl;
        exit(1);
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CascadeTrainer::runTrainCascadeCommand(void) const
{
    std::ostringstream command;
    command << "opencv_traincascade -data "<< cv::utils::fs::getcwd() << "/model -vec pos-samples.vec -bg negatives.txt -precalcValBufSize "
            <<  valBufSize << " -precalcIdxBufSize " << idxBufSize << " -numPos " << posRatio*nPos << " -numNeg " << nNegs << " -numStages "
            << nStages << " -minHitRate 0.99 -maxFalseAlarmRate 0.5 -w " << width << " -h " << height << " -featureType " << featureType << std::endl;

    system(command.str().c_str());
}
