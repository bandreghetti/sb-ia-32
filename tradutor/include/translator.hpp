#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <regex>
#include <unordered_set>

#include <utils.hpp>

class Translator {
    private:
        std::regex intRegEx = std::regex("(\\+|-)?[0-9]+");
        std::regex hexRegEx = std::regex("0(x|X)[0-9a-f]{1,4}");
        std::regex natRegEx = std::regex("[0-9]+");
        std::string fileName;
        std::list<std::tuple<int, std::list<std::string>>> srcLines;
        std::list<std::string> outLines;
        std::unordered_set<std::string> includeProcedures;
        std::string errMsg;
        std::string genErrMsg(int, std::string);
        int error = 0;
    public:
        Translator(std::string, std::list<std::tuple<int, std::list<std::string>>>);
        int printSource();
        int printOutput();
        int writeOutput();
        int translate();
        int getError();
        std::string getErrorMessage();
};
