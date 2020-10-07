#include "utils.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool doesExist(const std::string& dirName)
{
    return cv::utils::fs::exists(dirName);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void createDirectory(const std::string& dirName)
{
    if (!doesExist(dirName))
        cv::utils::fs::createDirectory(dirName);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void removeDirectory(const std::string& dirName)
{
    if (doesExist(dirName))
        cv::utils::fs::remove_all(dirName);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector<cv::String> listFiles(const std::string& dirName, const std::vector<std::string>& extensions, bool relative)
{
    std::vector<cv::String> fileNames;

    for (const auto& extension:extensions)
    {
        std::vector<cv::String> temp;

        if (relative==true)
            cv::utils::fs::glob_relative(dirName, extension, temp);
        else
            cv::utils::fs::glob(dirName, extension, temp);

        fileNames.insert(fileNames.end(), temp.begin(), temp.end());
    }

    return fileNames;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string selectFolder(const std::string& title)
{
    std::ostringstream command;
    command << "zenity --title \"" << title << "\" --file-selection --directory";

    FILE *pipe = popen(command.str().c_str(), "r");

    char buffer[1024];

    if (fgets(buffer, 1024, pipe) != NULL)
    {
        std::string foldername;
        foldername+=buffer;
        foldername.pop_back(); //remove the \n at the end of the folder name
        return foldername;
    }

    else
    {
        std::cerr << "No Folder Selected!" << std::endl;
        exit(1);
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string selectFile(const std::string& title, const std::vector<std::string>& extensions)
{
    std::ostringstream command;
    command << "zenity --title \"" << title << "\" --file-selection --file-filter=\"";

    for (const auto& extension:extensions)
        command << "\"" << extension << "\" ";

    command << "\"";

    FILE *pipe = popen(command.str().c_str(), "r");

    char buffer[1024];

    if (fgets(buffer, 1024, pipe) != NULL)
    {
        std::string filename;
        filename+=buffer;
        filename.pop_back(); //remove the \n at the end of the filename
        return filename;
    }

    else
    {
        std::cerr << "No File Selected!" << std::endl;
        exit(1);
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string currentDateTime(void)
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", &tstruct);

    return buf;
}
