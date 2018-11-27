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

    outFile << "; Auto-generated from " + fileName + ".asm\n";

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
                outLines.push_back("section .text");
                if (!hadText) {
                    outLines.push_back("global _start");
                    outLines.push_back("_start:");
                }
                hadText = true;
            } else if (line.back() == "DATA") {
                section = DATA;
                outLines.push_back("section .data");
            } else if (line.back() == "BSS") {
                section = BSS;
                outLines.push_back("section .bss");
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
                        outLine += ("resd " + *nextTokenIt);
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
            } else if (token == "JMP") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "jmp " + *nextTokenIt;
            } else if (token == "JMPN") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "cmp eax, 0\n";
                outLine += "jl " + *nextTokenIt;
            } else if (token == "JMPP") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "cmp eax, 0\n";
                outLine += "jg " + *nextTokenIt;
            } else if (token == "JMPZ") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "cmp eax, 0\n";
                outLine += "je " + *nextTokenIt;
            } else if (token == "COPY") {
                auto firstOpIt = std::next(tokenIt);
                if (firstOpIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                auto secondOpIt = std::next(firstOpIt);
                if (secondOpIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                auto firstOp = *firstOpIt;
                auto secondOp = *secondOpIt;

                // Remove comma if needed
                if (isSuffix(firstOp, ",")) {
                    (firstOp).pop_back();
                }

                outLine += "mov ebx, dword[" + secondOp + "]\n";
                outLine += "mov [" + firstOp + "], ebx";
            } else if (token == "LOAD") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "mov eax, dword[" + *nextTokenIt + "]";
            } else if (token == "STORE") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "mov [" + *nextTokenIt + "], eax";
            } else if (token == "STOP") {
                outLine += "mov eax, 1\n";
                outLine += "mov ebx, 0\n";
                outLine += "int 80h";
            } else if (token == "MULT") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "mov ebx, dword[" + *nextTokenIt + "]\n";
                outLine += "imul ebx";
            } else if (token == "DIV") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "mov ebx, dword[" + *nextTokenIt + "]\n";
                outLine += "cdq\n";
                outLine += "idiv ebx";
            } else if (token == "INPUT") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "push " + *nextTokenIt + "\n";
                outLine += "call INPUT";
                includeProcedures.insert("INPUT");
            } else if (token == "OUTPUT") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "push " + *nextTokenIt + "\n";
                outLine += "call OUTPUT";
                includeProcedures.insert("OUTPUT");
            } else if (token == "C_INPUT") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "push " + *nextTokenIt + "\n";
                outLine += "call C_INPUT";
                includeProcedures.insert("C_INPUT");
            } else if (token == "C_OUTPUT") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                outLine += "push " + *nextTokenIt + "\n";
                outLine += "call C_OUTPUT";
                includeProcedures.insert("C_OUTPUT");
            } else if (token == "S_INPUT") {
                auto firstOpIt = std::next(tokenIt);
                if (firstOpIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                auto secondOpIt = std::next(firstOpIt);
                if (secondOpIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                auto firstOp = *firstOpIt;
                auto secondOp = *secondOpIt;

                // Remove comma if needed
                if (isSuffix(firstOp, ",")) {
                    (firstOp).pop_back();
                }

                outLine += "push " + firstOp + "\n";
                outLine += "push " + secondOp + "\n";
                outLine += "call S_INPUT";
                includeProcedures.insert("S_INPUT");
            } else if (token == "S_OUTPUT") {
                auto firstOpIt = std::next(tokenIt);
                if (firstOpIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                auto secondOpIt = std::next(firstOpIt);
                if (secondOpIt == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting label, found newline");
                    return error;
                }
                auto firstOp = *firstOpIt;
                auto secondOp = *secondOpIt;

                // Remove comma if needed
                if (isSuffix(firstOp, ",")) {
                    (firstOp).pop_back();
                }

                outLine += "push " + firstOp + "\n";
                outLine += "push " + secondOp + "\n";
                outLine += "call S_OUTPUT";
                includeProcedures.insert("S_OUTPUT");
            } else {
                // Invalid instruction
                errMsg = genErrMsg(lineCount, "unknown instruction/directive " + token);
                return error;
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

    if (!includeProcedures.empty()) {
        outLines.push_back("section .text");
        for (auto procedure : includeProcedures) {
            std::string procLines;
            if (procedure == "INPUT") {
                procLines += "INPUT:\n";
                procLines += "enter 0,0\n";

            } else if (procedure == "OUTPUT") {
                procLines += "OUTPUT:\n";
                procLines += "enter 0,0\n";

            } else if (procedure == "C_INPUT") {
                procLines += "C_INPUT:\n";
                procLines += "enter 0,0\n";
                procLines += "mov ecx, [ebp+8]\n";
                procLines += "mov eax, 3\n";
                procLines += "mov ebx, 0\n";
                procLines += "mov edx, 1\n";
                procLines += "int 0x80\n";
            } else if (procedure == "C_OUTPUT") {
                procLines += "C_OUTPUT:\n";
                procLines += "enter 0,0\n";
                procLines += "mov ecx, [ebp+8]\n";
                procLines += "mov eax, 4\n";
                procLines += "mov ebx, 1\n";
                procLines += "mov edx, 1\n";
                procLines += "int 0x80\n";
            } else if (procedure == "S_INPUT") {
                procLines += "S_INPUT:\n";
                procLines += "enter 0,0\n";
                procLines += "mov ebx, 0\n";
                procLines += "mov edx, 1\n";
                procLines += "mov ecx, dword[ebp+12]\n";
                procLines += "sin_l1: mov eax, 3\n";
                procLines += "push ecx\n";
                procLines += "mov ecx, dword[esp+12]\n";
                procLines += "int 0x80\n";
                procLines += "mov eax, ecx\n";
                procLines += "inc ecx\n";
                procLines += "cmp byte[ecx], 0xa\n";
                procLines += "mov dword[esp+12], ecx\n";
                procLines += "pop ecx\n";
                procLines += "loopne sin_l1\n";
            } else if (procedure == "S_OUTPUT") {
                procLines += "S_OUTPUT:\n";
                procLines += "enter 0,0\n";
                procLines += "mov ebx, 1\n";
                procLines += "mov edx, 1\n";
                procLines += "mov ecx, dword [ebp+12]\n";
                procLines += "sout_l1: \n";
                procLines += "mov eax, 4\n";
                procLines += "push ecx\n";
                procLines += "mov ecx, dword[esp+12]\n";
                procLines += "int 0x80\n";
                procLines += "mov eax, ecx\n";
                procLines += "inc ecx\n";
                procLines += "cmp byte[ecx], 0xa\n";
                procLines += "mov dword[esp+12], ecx\n";
                procLines += "pop ecx\n";
                procLines += "loopne sout_l1\n";
            }
            procLines += "leave\n";
            procLines += "ret";
            outLines.push_back(procLines);
        }
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
