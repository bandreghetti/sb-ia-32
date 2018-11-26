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

class Translator {
    private:
        std::regex intRegEx = std::regex("(\\+|-)?[0-9]+");
        std::regex hexRegEx = std::regex("0(x|X)[0-9a-f]{1,4}");
        std::regex natRegEx = std::regex("[0-9]+");
        std::string fileName;
        std::list<std::tuple<int, std::list<std::string>>> srcLines;
        std::list<std::string> outLines;
        std::map<std::string, int> symbolsMap;
        std::set<std::string> externSymbols;
        std::list<std::tuple<std::string, int>> useTable;
        std::list<std::tuple<std::string, int>> definitionTable;
        std::list<unsigned int> relative;
        std::list<short> machineCode;
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
            C_INPUT,
            C_OUTPUT,
            S_INPUT,
            S_OUTPUT,
        };
        const std::map<std::string, short> opcodeMap = {
            {"ADD", ADD},
            {"SUB", SUB},
            {"MULT", MULT},
            {"DIV", DIV},
            {"JMP", JMP},
            {"JMPN", JMPN},
            {"JMPP", JMPP},
            {"JMPZ", JMPZ},
            {"COPY", COPY},
            {"LOAD", LOAD},
            {"STORE", STORE},
            {"INPUT", INPUT},
            {"OUTPUT", OUTPUT},
            {"STOP", STOP},
            {"C_INPUT", STOP},
            {"C_OUTPUT", C_OUTPUT},
            {"S_INPUT", S_INPUT},
            {"S_OUTPUT", S_OUTPUT},
        };
        int error = 0;
        std::string errMsg;
        std::string genErrMsg(int, std::string);
        void handleArgument(int, std::list<std::string>::iterator*, std::list<std::string>::iterator, int*);
    public:
        Translator(std::string, std::list<std::tuple<int, std::list<std::string>>>);
        int printSource();
        int printOutput();
        int writeOutput();
        int translate();
        int getError();
        std::string getErrorMessage();
};
