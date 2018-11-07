#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <tuple>

#include <utils.hpp>

class PreProcessor {
   private:
    std::string fileName;
    std::list<std::tuple<int, std::string>> srcLines;
    std::list<std::tuple<int, std::list<std::string>>> outLines;
    int error = 0;
   public:
    PreProcessor(std::string);
    ~PreProcessor();
    int printSource();
    int printOutput();
    int writeOutput();
    int preProcess();
    int getError();
    std::list<std::tuple<int, std::list<std::string>>> getOutput();
};
