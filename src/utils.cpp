#include "utils.h"
#include <iostream>
#include <filesystem>

using std::cout, std::endl;
namespace fs = std::filesystem;

//https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
//because c++ string library doesn't provide it...
std::vector<std::string> split(const std::string& s, char delimiter){
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
      tokens.push_back(token);
   return tokens;
}

string cleanFilename(const string &filename){
    const auto filenameSplit1 = split(filename, '/'); //!fixme: hardcoded linux path sep.
    const auto filenameSplit2 = split(filenameSplit1.back(), '.');
    return filenameSplit2[0];
}

void createFolderIfDoesntExist(const string &folderName){
    if(!fs::is_directory(folderName)){
        if(!fs::exists(folderName)){
            cout<<"Creating "<<folderName<<" folder"<<endl;
            fs::create_directory(folderName);
        }
        else throw folderName + " is not a folder";
    }
}
