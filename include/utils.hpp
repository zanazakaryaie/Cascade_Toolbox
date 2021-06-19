#pragma once

#include <vector>
#include <string>

bool doesExist(const std::string& dirName); //Checks if directory exists

void createDirectory(const std::string& dirName); //Creates directory if it does not exist

void removeDirectory(const std::string& dirName); //Removes directory if it exists

std::vector<std::string> listFiles(const std::string& dirName, const std::vector<std::string>& extensions, bool relative=true); //Lists all files in dirName with extensions.

std::string selectFile(const std::string& title, const std::vector<std::string>& extensions); //Opens dialog box for the user to select files with specified extensions

std::string selectFolder(const std::string& title); //Opens dialog box for the user to select a folder

std::string currentDateTime(void); //Returns current date and time. Used for naming samples uniquely


