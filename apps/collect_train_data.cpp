#include <iostream>
#include "Annotator.hpp"

using namespace std;

void printHelp(void)
{
    cout << "Help" << endl;
    cout << "==================================================" << endl;
    cout << "1- Select a video file when the dialog box pops-up" << endl;
    cout << "2- Left-click and drag to draw an object" << endl;
    cout << "3- Left-click on the anchors and and drag to edit the object" << endl;
    cout << "4- Right-click in an object to remove it" << endl;
    cout << "5- Press space to go to the next 10th frame" << endl;
    cout << "6- Press + and - to increase and decrease the step when space is pressed" << endl;
    cout << "7- Press q to exit annotating" << endl;
    cout << "==================================================" << endl;
}

int main(void)
{
    printHelp();

    Annotator annot(TRAINING);

    annot.run();

    return 0;
}
