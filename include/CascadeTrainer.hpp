#pragma once

#include <fstream>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

class CascadeTrainer
{
public:

    CascadeTrainer(void); //Constructor
    void start(void); //Trains the model. If an error occurs for any reason, the model is generated up to the last available stage.


private:

    void checkPreviousTraining(void) const; //Asks the user to whether continue with previous trained model (if any), or start a new model
    void readConfigs(void); //Asks the user to select .yaml config file. Reads the cascade model configs
    void generatePosVecFile(void); //Asks the user to select positive directory. Generates .vec file from positive images in the directory
    void generateNegTxtFile(void); //Asks the user to select negative directory. Generates .txt file from negative images in the directory
    void runCreateSampleCommand(void) const; //Calls opencv_createsamples application
    void runTrainCascadeCommand(void) const; //Calls opencv_traincascade application


    //Configurations
    int width;
    int height;
    std::string featureType; //HAAR | LBP
    int nStages;
    int valBufSize;
    int idxBufSize;
    float posRatio; //(0-1)

    int nPos, nNegs;
};
