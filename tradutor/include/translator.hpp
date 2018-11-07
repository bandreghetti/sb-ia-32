#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <regex>
#include <set>

#include <utils.hpp>

class Assembler {
    private:
        std::regex intRegEx = std::regex("(\\+|-)?[0-9]+");
        std::regex hexRegEx = std::regex("0(x|X)[0-9a-f]{1,4}");
        std::regex natRegEx = std::regex("[0-9]+");
        std::string fileName;
        std::list<std::tuple<int, std::list<std::string>>> srcLines;
        std::map<std::string, int> symbolsMap;
        std::set<std::string> externSymbols;
        std::list<std::tuple<std::string, int>> useTable;
        std::list<std::tuple<std::string, int>> definitionTable;
        std::list<unsigned int> relative;
        std::list<short> machineCode;
        std::set<std::string> zeroList;
        std::set<std::string> invalidJumpList;
        bool isModule = false;
        enum {
            ADD = 1,
            SUB,
            MULT,
            DIV,
            JMP,
            JMPN,
            JMPP,
            JMPZ,
            COPY,
            LOAD,
            STORE,
            INPUT,
            OUTPUT,
            STOP,
        };
        const std::map<std::string, short> opcodeMap = {
            {"ADD", 1},
            {"SUB", 2},
            {"MULT", 3},
            {"DIV", 4},
            {"JMP", 5},
            {"JMPN", 6},
            {"JMPP", 7},
            {"JMPZ", 8},
            {"COPY", 9},
            {"LOAD", 10},
            {"STORE", 11},
            {"INPUT", 12},
            {"OUTPUT", 13},
            {"STOP", 14}
        };
        const std::map<std::string, int> memSpaceMap = {
            {"ADD", 2},
            {"SUB", 2},
            {"MULT", 2},
            {"DIV", 2},
            {"JMP", 2},
            {"JMPN", 2},
            {"JMPP", 2},
            {"JMPZ", 2},
            {"COPY", 3},
            {"LOAD", 2},
            {"STORE", 2},
            {"INPUT", 2},
            {"OUTPUT", 2},
            {"STOP", 1},
            {"PUBLIC", 0}
        };
        int error = 0;
        std::string errMsg;
        std::string genErrMsg(int, std::string);
        void handleArgument(int, std::list<std::string>::iterator*, std::list<std::string>::iterator, int*);
    public:
        Assembler(std::string, std::list<std::tuple<int, std::list<std::string>>>);
        int printSource();
        int printOutput();
        int writeOutput();
        int firstPass();
        int secondPass();
        int getError();
        std::string getErrorMessage();
};
