#include <translator.hpp>

Translator::Translator(std::string fileName, std::list<std::tuple<int, std::list<std::string>>> srcLines) {
    this->fileName = fileName;
    this->srcLines = srcLines;
}

int Translator::printSource() {
    if (error != 0) {
        return error;
    }
    for (auto lineTuple : srcLines) {
        printf("%3d:", std::get<0>(lineTuple));
        auto tokens = std::get<1>(lineTuple);
        for (auto token : tokens) {
            printf(" %s", token.c_str());
        }
        printf("\n");
    }
    return 0;
}

int Translator::writeOutput() {
    if (error != 0) {
        return error;
    }
    std::ofstream outFile;
    outFile.open(fileName + ".s");

    outFile << "; Auto-generated from " + fileName + ".asm";

    for (auto line : outLines) {
        outFile << line + '\n';
    }

    outFile.close();

    return 0;
}

enum {
    NONE = 0,
    TEXT,
    DATA,
    BSS
};

int Translator::translate() {
    if (error != 0) {
        return error;
    }
    int memCount = 0;
    int section = NONE;
    bool moduleEnded = false;
    bool hadText = false;

    for (auto lineIt = srcLines.begin(); lineIt != srcLines.end(); ++lineIt)
    {
        std::string outLine;
        auto line = std::get<1>(*lineIt);
        auto lineCount = std::get<0>(*lineIt);

        // Handle section change
        if (line.front() == "SECTION") {
            // Section lines must always have 2 tokens (SECTION <section_name>)
            if (line.size() != 2) {
                errMsg = genErrMsg(lineCount, "section lines must always have 2 tokens");
                error = 1;
                return error;
            }
            // Change the section variable according to the second token
            if (line.back() == "TEXT") {
                section = TEXT;
                outLines.push_back(".section text");
                if (!hadText) {
                    outLines.push_back("global _start");
                    outLines.push_back("_start:");
                }
                hadText = true;
            } else if (line.back() == "DATA") {
                section = DATA;
                outLines.push_back(".section data");
            } else if (line.back() == "BSS") {
                section = BSS;
                outLines.push_back(".section bss");
            } else {
                errMsg = genErrMsg(lineCount, "unknown section name " + line.back());
                error = 1;
                return error;
            }
            continue;
        }

        // Get iterator to the first token in line
        auto tokenIt = line.begin();

        // If line begins with label
        auto label = line.front();
        if (isSuffix(label, ":")) {
            if (section == TEXT) {
                outLines.push_back(label);
            } else {
                label.pop_back();
                outLine += label + " ";
            }
            // Advance to second token in line
            ++tokenIt;
        }

        if (tokenIt != line.end()) {
            auto token = *tokenIt;
            if (token == "SPACE") {
                // Check whether SPACE was given an argument or not
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt != line.end()) {
                    // Since an argument was given, check if it is valid
                    if (std::regex_match(*nextTokenIt, natRegEx)) {
                        outLine += ("resd " + std::stoi(*nextTokenIt));
                    } else {
                        errMsg = genErrMsg(lineCount, "invalid argument for SPACE directive: " + *nextTokenIt);
                        error = 1;
                        return error;
                    }
                } else {
                    // Since no argument was given, reserve only one space
                    outLine += "resd 1";
                }
            } else if (token == "CONST") {
                // Check whether CONST was given an argument or not
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting decimal or hexadecimal number, found newline");
                    return error;
                }

                int constVal;
                // Check if given argument is decimal or hexadecimal
                if(std::regex_match(*nextTokenIt, intRegEx)) {
                    constVal = std::stoi(*nextTokenIt);
                } else if (std::regex_match(*nextTokenIt, hexRegEx)) {
                    // Convert to lower case if hexadecimal
                    for(auto &c : *nextTokenIt) {
                        c = std::tolower(c);
                    }
                    constVal = std::stoi(*nextTokenIt, 0, 16);
                } else {
                    errMsg = genErrMsg(lineCount, "invalid immediate " + *tokenIt);
                    error = 1;
                    return error;
                }
                outLine += "dd " + std::to_string(constVal);
            } else if (token == "ADD") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "add eax, dword[" + *nextTokenIt + "]";
            } else if (token == "SUB") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "sub eax, dword[" + *nextTokenIt + "]";
            } else if (token == "MULT") {
            } else if (token == "DIV") {
            } else {
                // Invalid instruction
                // errMsg = genErrMsg(lineCount, "unknown instruction/directive " + token);
                // return error;
            }
        }

        if (outLine.length() > 0) {
            outLines.push_back(outLine);
        }
    }

    // Check if TEXT section was declared
    if (!hadText) {
        errMsg = "TEXT section not found";
        error = 1;
        return error;
    }

    return 0;
}

std::string Translator::getErrorMessage() {
    return errMsg;
}

int Translator::getError() {
    return error;
}

std::string Translator::genErrMsg(int lineCount, std::string message) {
    error = 1;
    return "line " + std::to_string(lineCount) + ": " + message;
}

void Translator::handleArgument(int lineCount, std::list<std::string>::iterator* tokenItPtr, std::list<std::string>::iterator lineEnd, int* memCountPtr) {
    auto operand = **tokenItPtr;

    // Check if operator is COPY (takes two arguments, need to handle comma)
    bool isCopy = false;
    if (*std::prev(*tokenItPtr) == "COPY") {
        isCopy = true;
        // Remove comma if needed
        if (isSuffix(operand, ",")) {
            operand.pop_back();
        }
    }

    // Check if argument is defined in symbols map
    if (symbolsMap.count(operand) == 0) {
        errMsg = genErrMsg(lineCount, "unknown symbol " + operand);
        error = 1;
        return;
    }
    auto memOperand = symbolsMap.at(operand);

    // Handle LABEL + N case
    if (std::next(*tokenItPtr) != lineEnd && symbolsMap.count(*std::next(*tokenItPtr)) == 0) {
        ++*tokenItPtr;
        // Check if next token is a +
        auto plusSign = **tokenItPtr;
        if (plusSign != "+") {
            if (isCopy) {
                errMsg = genErrMsg(lineCount, "expecting + or known symbol, found " + plusSign);
            } else {
                errMsg = genErrMsg(lineCount, "expecting + or newline, found " + plusSign);
            }
            error = 1;
            return;
        }
        // Check if there is a token after +
        if (std::next(*tokenItPtr) == lineEnd) {
            errMsg = genErrMsg(lineCount, "expecting decimal number, found newline");
            error = 1;
            return;
        }
        ++*tokenItPtr;
        auto N = **tokenItPtr;
        // Handle comma if necessary
        if (isCopy && isSuffix(N, ",")) {
            N.pop_back();
        }
        // Check if token after + is a valid number
        if (!std::regex_match(N, natRegEx)) {
            errMsg = genErrMsg(lineCount, "expecting decimal number, found " + N);
            error = 1;
            return;
        }
        // Since everything went ok, increment memOperand
        memOperand += std::atoi(N.c_str());
    }
    // Include relative symbol address to machine code
    machineCode.push_back(memOperand);
    // If operand is an extern symbol, add its address to use table
    if (externSymbols.count(operand) > 0) {
        auto useTuple = std::make_tuple(operand, *memCountPtr);
        useTable.push_back(useTuple);
    // Else, add its address to relative list
    } else {
        relative.push_back(*memCountPtr);
    }
    ++*memCountPtr;
}
