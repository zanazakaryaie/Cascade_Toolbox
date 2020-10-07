#include <iostream>
#include "Annotator.hpp"

using namespace std;

void printHelp(void)
{
    cout << "Select a video file when the dialog box pops-up" << endl;
    cout << "Left-click and drag to draw an object" << endl;
    cout << "Left-click on the anchors and and drag to edit the object" << endl;
    cout << "Right-click in an object to remove it" << endl;
    cout << "Press space to go to the next 10th frame" << endl;
    cout << "Press + and - to increase and decrease the step when space is pressed" << endl;
    cout << "Press q to exit annotating" << endl;
}

int main(int argc, char* argv[])
{
    if (argc==2 && (strcmp(argv[1], "--help") || strcmp(argv[1], "-help")))
    {
        printHelp();
        return 0;
    }

    Annotator annot(HARD_MINING);

    annot.run();

    return 0;
}
