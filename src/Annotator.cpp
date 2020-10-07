#include "Annotator.hpp"


//Initializing static variables
bool Annotator::drawing;
bool Annotator::moving;
bool Annotator::editing;
cv::Point2i Annotator::p1;
std::vector<BoundingBox> Annotator::objects;
cv::Mat Annotator::frame;
cv::Mat Annotator::frameWithObjects;
int Annotator::anchorIdx;
int Annotator::objectIdx;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Annotator::Annotator(ANNOT_MODE m)
{
    mode = m;

    selectVideoFile();

    if (mode==TRAINING || mode==HARD_MINING)
    {
        createDirectory("Negatives");
        createDirectory("Positives");
        sumWidth = 0;
        sumHeight = 0;
    }

    if (mode==HARD_MINING)
        selectCascadeModel();

    if (mode==TESTING)
        createDirectory("TestData");

    drawing = false;
    moving = false;
    editing = false;

    frameSteps = 10;
    rng(0xFFFFFFFF);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::run(void)
{
    while (cap.isOpened())
    {
        cap.read(frame);

        if (frame.empty())
            break;

        if (mode==HARD_MINING)
            detectObjects();

        frame.copyTo(frameWithObjects);

        while (true)
        {
            cv::imshow("Draw objects", frameWithObjects);
            cv::setMouseCallback("Draw objects", mouseCallBackFunc, NULL);
            drawObjects();

            if (checkKeyPressed())
                break;
        }
    }

    cap.release();

    if (mode==TRAINING || mode==HARD_MINING)
        std::cout << "Average width/height: " << 1.f*sumWidth/sumHeight << std::endl;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::detectObjects(void)
{
    std::vector<cv::Rect> detectedBoxes;
    detector.detectMultiScale(frame, detectedBoxes);

    for (const auto& box : detectedBoxes)
        objects.push_back(BoundingBox(box));
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int Annotator::checkKeyPressed(void)
{
    char key = cv::waitKey(20);

    if (key=='+')
    {
        std::cout << "Frame steps: " << ++frameSteps << std::endl;
    }

    else if (key=='-')
    {
        if (frameSteps>1)
        {
            std::cout << "Frame steps: " << --frameSteps << std::endl;
        }
    }

    else if (key=='q')
    {
        cap.release();
        exit(1);
    }

    else if (key==' ')
    {
        saveSamples();

        for (int i=1; i<frameSteps; i++)
        {
            if (!cap.grab())
            {
                std::cerr << "No more frames to grab! Reached the end of the video file" << std::endl;
                cap.release();
                exit(1);
            }
        }

        return 1;
    }

    return 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::selectVideoFile(void)
{
    std::string filename = selectFile("Choose the video file", {"*.mp4", "*.avi"});
    cap.open(filename);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::selectCascadeModel(void)
{
    std::string filename = selectFile("Choose the cascade model", {"*.xml"});
    detector.load(filename);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::mouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    cv::Point2i p = cv::Point2i(x,y);

    if  (event == cv::EVENT_LBUTTONDOWN )
        checkLeftButtonDown(p);

    else if (event == cv::EVENT_MOUSEMOVE)
        checkMouseMove(p);

    else if (event == cv::EVENT_LBUTTONUP )
        checkLeftButtonUp(p);

    else if ( event == cv::EVENT_RBUTTONDOWN )
        removeObject(p);

    else if  ( event == cv::EVENT_MBUTTONDOWN )
        checkMiddleButtonDown(p);

    else if (moving &&  event == cv::EVENT_MBUTTONUP )
        moving = false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::checkLeftButtonDown(const cv::Point2i p)
{
    p1 = p;
    objectIdx = isAnchor(p1, anchorIdx);

    if (objectIdx>=0)
    {
        editing = true;
    }
    else
    {
        drawing = true;
        objects.push_back(BoundingBox(p1,p1));
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::checkMouseMove(const cv::Point2i p)
{
    if (drawing)
    {
        objects.back() = BoundingBox(p1, p);
    }
    else if (moving)
    {
        objects[objectIdx].move(p.x-p1.x, p.y-p1.y);
        p1.x = p.x;
        p1.y = p.y;
    }
    else if (editing)
    {
        editBox(objects[objectIdx], anchorIdx, p);
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::checkLeftButtonUp(const cv::Point2i p)
{
    if (drawing)
    {
        drawing = false;

        if (abs(p.x-p1.x)<10 && abs(p.y-p1.y)<10)
        {
            std::cout << "Objects smaller than 10x10 pixels are removed automatically" << std::endl;
            objects.pop_back();
        }
    }
    else if (editing)
    {
        editing = false;
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::checkMiddleButtonDown(const cv::Point2i p)
{
    p1 = p;

    for (size_t i=0; i<objects.size(); i++)
    {
        if (objects[i].doesContain(p1))
        {
            objectIdx = i;
            moving = true;
            break;
        }
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::editBox(BoundingBox& box, int anchorNo, const cv::Point2i p)
{
    int x = p.x;
    int y = p.y;

    switch (anchorNo)
    {
    case 0:
        box.setTopLeftCorner(p);
        break;
    case 1:
        box.setTopLeftCorner(cv::Point2i(box.tl.x,y));
        break;
    case 2:
        box.setTopLeftCorner(cv::Point2i(box.tl.x,y));
        box.setBottomRightCorner(cv::Point2i(x,box.br.y));
        break;
    case 3:
        box.setTopLeftCorner(cv::Point2i(x,box.tl.y));
        break;
    case 4:
        box.setBottomRightCorner(cv::Point2i(x,box.br.y));
        break;
    case 5:
        box.setTopLeftCorner(cv::Point2i(x,box.tl.y));
        box.setBottomRightCorner(cv::Point2i(box.br.x,y));
        break;
    case 6:
        box.setBottomRightCorner(cv::Point2i(box.br.x, y));
        break;
    case 7:
        box.setBottomRightCorner(p);
        break;
    }

    if (box.mode==DETECTED)
        box.mode = DRAWN; //when a detected box is edited by user, it must be saved as a new positive in saveSamples() function. So, the mode is changed to DRAWN
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int Annotator::isAnchor(const cv::Point2i p, int &anchorNo)
{
    for (size_t i=0; i<objects.size(); i++)
    {
        const BoundingBox& object = objects[i];

        if (abs(p.x - object.tl.x)<10 && abs(p.y - object.tl.y)<10)
        {
            anchorNo = 0; //Top-left
            return i;
        }
        else if (abs(p.x - object.tm.x)<10 && abs(p.y - object.tm.y)<10)
        {
            anchorNo = 1; //Top-middle
            return i;
        }
        else if (abs(p.x - object.tr.x)<10 && abs(p.y - object.tr.y)<10)
        {
            anchorNo = 2; //Top-right
            return i;
        }
        else if (abs(p.x - object.ml.x)<10 && abs(p.y - object.ml.y)<10)
        {
            anchorNo = 3; //Middle-left
            return i;
        }
        else if (abs(p.x - object.mr.x)<10 && abs(p.y - object.mr.y)<10)
        {
            anchorNo = 4; //Middle-right
            return i;
        }
        else if (abs(p.x - object.bl.x)<10 && abs(p.y - object.bl.y)<10)
        {
            anchorNo = 5; //Bottom-left
            return i;
        }
        else if (abs(p.x - object.bm.x)<10 && abs(p.y - object.bm.y)<10)
        {
            anchorNo = 6; //Bottom-middle
            return i;
        }
        else if (abs(p.x - object.br.x)<10 && abs(p.y - object.br.y)<10)
        {
            anchorNo = 7; //Bottom-right
            return i;
        }
    }

    return -1;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::saveSamples(void)
{
    if (mode==TRAINING) //HARD_MINING and TESTING modes do not require negative patch generation
        saveNegatives();

    if (mode==TESTING)
        saveTestData();
    else //TRAINING or HARD_MINING
        saveTrainData();

    objects.clear();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::saveNegatives(void)
{
    std::vector<cv::Rect> negatives = extractRandomNegatives();
    for (size_t i=0; i<negatives.size(); i++)
    {
        std::ostringstream name;
        name << "Negatives/" << currentDateTime() << "_" << i << ".png";
        cv::imwrite(name.str(), frame(negatives[i]));
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::saveTestData(void) const
{
    if (objects.size()>0)
    {
        cv::Rect frameRect = cv::Rect(0,0,frame.cols, frame.rows);

        int frameIdx = cap.get(cv::CAP_PROP_POS_FRAMES);
        std::ostringstream name;
        name << "TestData/" << frameIdx-1 << ".txt";
        std::ofstream test_data(name.str());

        if (test_data.is_open())
        {
            for (const auto& object : objects)
            {
                cv::Rect intersection = frameRect & object.getRect(); //Avoid crash when a part of the object is outside the frame
                test_data << intersection.x << " " << intersection.y << " " << intersection.width << " " << intersection.height << std::endl;
            }

            test_data.close();
        }
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::saveTrainData(void)
{
    cv::Rect frameRect = cv::Rect(0,0,frame.cols, frame.rows);

    for (size_t i=0; i<objects.size(); i++)
    {
        cv::Rect intersection = frameRect & objects[i].getRect(); //Avoid crash when a part of the object is outside the frame

        if (objects[i].mode==DRAWN)
        {
            std::ostringstream name;
            name << "Positives/" << currentDateTime() << "_" << i << ".png";
            cv::imwrite(name.str(), frame(intersection));
        }
        else if (objects[i].mode==HIDDEN)
        {
            std::ostringstream name;
            name << "Negatives/" << currentDateTime() << "_" << i << ".png";
            cv::imwrite(name.str(), frame(intersection));
        }

        sumWidth+=intersection.width;
        sumHeight+=intersection.height;
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector<cv::Rect> Annotator::extractRandomNegatives(void)
{
    std::vector<cv::Rect> negatives;

    const size_t N = 2*objects.size(); //A rule of thumb is to have twice negatives

    while (negatives.size() < N)
    {
        int x = rng.uniform(0, frame.cols);
        int y = rng.uniform(0, frame.rows);
        int width = rng.uniform(0, frame.cols-x);
        int height = rng.uniform(0, frame.rows-y);

        if (width<10 || height<10) //Regions smaller than 10x10 pixels are removed
            continue;

        float aspectRatio = 1.f*width/height;
        if (aspectRatio>5 || aspectRatio<0.2) //Very long or wide regions are not good
            continue;

        cv::Rect2i negativeCandidate = cv::Rect2i(x,y,width,height);

        int intersection = 0;
        for (const auto& object : objects)
            intersection+=(negativeCandidate & object.getRect()).area();

        if (intersection==0) //No intersection with positives
        {
            intersection = 0;
            for (const auto& negative : negatives)
                intersection+=(negativeCandidate & negative).area();

            if (intersection==0) //No intersection with negatives
                negatives.push_back(negativeCandidate);
        }
    }

    return negatives;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::removeObject(const cv::Point2i p)
{
    for (size_t i=0; i<objects.size(); i++)
    {
        BoundingBox &object = objects[i];

        if (object.mode!=HIDDEN && object.doesContain(p))
        {
            if (object.mode==DRAWN)
                objects.erase(objects.begin()+i);
            else //mode=DETECTED
                object.mode=HIDDEN;

            break;
        }
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Annotator::drawObjects(void)
{
    frame.copyTo(frameWithObjects);

    for (const auto &object : objects)
        if (object.mode!=HIDDEN)
            object.drawOn(frameWithObjects);
}
