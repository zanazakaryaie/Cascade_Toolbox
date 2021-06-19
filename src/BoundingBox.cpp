#include "BoundingBox.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BoundingBox::BoundingBox(const cv::Point2i p1, const cv::Point2i p2, BOX_MODE m)
{
    int min_x = std::min(p1.x, p2.x);
    int min_y = std::min(p1.y, p2.y);

    tl = cv::Point2i(min_x, min_y);

    _width = abs(p1.x-p2.x);
    _height = abs(p1.y-p2.y);

    calcAnchros();

    mode = m;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BoundingBox::BoundingBox(const cv::Rect rectangle, BOX_MODE m)
{
    tl = cv::Point2i(rectangle.x, rectangle.y);

    _width = rectangle.width;
    _height = rectangle.height;

    calcAnchros();

    mode = m;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BoundingBox::setTopLeftCorner(const cv::Point2i p)
{
    _width += tl.x-p.x;
    _height += tl.y-p.y;

    tl = p;

    calcAnchros();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BoundingBox::setBottomRightCorner(const cv::Point2i p)
{
    _width += p.x-br.x;
    _height += p.y-br.y;

    calcAnchros();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Rect BoundingBox::getRect(void) const
{
    return cv::Rect(tl,br);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool BoundingBox::doesContain(const cv::Point2i p) const
{
    if (tl.x<=p.x && p.x<=br.x && tl.y<=p.y && p.y<=br.y)
        return true;

    return false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BoundingBox::move(int deltaX, int deltaY)
{
    tl.x+=deltaX;
    tl.y+=deltaY;

    calcAnchros();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BoundingBox::drawOn(cv::Mat &frame) const
{
    auto copiedFrame = frame.clone();
    cv::rectangle(copiedFrame, getRect(), cv::Scalar(0,255,0), -1);
    cv::addWeighted(copiedFrame, 0.3, frame, 0.7, 0, frame);

    const auto shift = cv::Point2i(5,5);

    //Anchors
    cv::rectangle(frame, cv::Rect2i(tl-shift, tl+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(tm-shift, tm+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(tr-shift, tr+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(ml-shift, ml+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(mr-shift, mr+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(bl-shift, bl+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(bm-shift, bm+shift), cv::Scalar(0,255,0), 2);
    cv::rectangle(frame, cv::Rect2i(br-shift, br+shift), cv::Scalar(0,255,0), 2);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BoundingBox::calcAnchros(void)
{
    br.x = tl.x+_width;
    br.y = tl.y+_height;

    tm.x = tl.x+(_width/2);
    tm.y = tl.y;

    tr.x = br.x;
    tr.y = tl.y;

    ml.x = tl.x;
    ml.y = tl.y+(_height/2);

    mr.x = br.x;
    mr.y = ml.y;

    bl.x = tl.x;
    bl.y = br.y;

    bm.x = tm.x;
    bm.y = br.y;
}
