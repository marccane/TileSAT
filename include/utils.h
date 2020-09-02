#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>

using std::vector, std::string;

vector<string> split(const string& s, char delimiter);
string cleanFilename(const string &filename);
void createFolderIfDoesntExist(const string &folderName);

#endif // UTILS_H
