#include <algorithm>
#include <filesystem>

#include "utils.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool doesExist(const std::string& dirName)
{
    return std::filesystem::exists(dirName);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void createDirectory(const std::string& dirName)
{
    if (!doesExist(dirName))
        std::filesystem::create_directory(dirName);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void removeDirectory(const std::string& dirName)
{
    if (doesExist(dirName))
        std::filesystem::remove_all(dirName);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector<std::string> listFiles(const std::string& dirName, const std::vector<std::string>& extensions, bool relative)
{
    std::vector<std::string> fileNames;

    for (const auto& p : std::filesystem::directory_iterator(dirName))
      if (std::find(extensions.begin(), extensions.end(), p.path().extension()) != extensions.end())
        fileNames.push_back((relative ? p.path().filename().string() : p.path().string()));

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
        throw std::runtime_error("No Folder Selected!");
    }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string selectFile(const std::string& title, const std::vector<std::string>& extensions)
{
    std::ostringstream command;
    command << "zenity --title \"" << title << "\" --file-selection --file-filter=\"";

    for (const auto& extension : extensions)
        command << "\"*" << extension << "\" ";

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
        throw std::runtime_error("No File Selected!");
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
