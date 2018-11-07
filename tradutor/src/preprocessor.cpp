#include <preprocessor.hpp>

PreProcessor::PreProcessor(std::string fileName) {
    this->fileName = fileName;
    std::string asmName = fileName + ".asm";  // input file name
    if (!fileExists(asmName)) {
        std::cout << "File " + asmName + " does not exists\n";
        error = -1;
        return;
    }

    // Read all lines from file
    std::ifstream srcFile;
    srcFile.open(asmName);
    std::string line;
    unsigned int lineCount = 1;
    while (getline(srcFile, line)) {
        auto lineTuple = std::make_tuple(lineCount, line);
        srcLines.push_back(lineTuple);
        ++lineCount;
    }
    srcFile.close();
}

PreProcessor::~PreProcessor() {}

int PreProcessor::printSource() {
    if (error != 0) {
        return error;
    }
    for (auto lineTuple : srcLines) {
        printf("%3d:%s\n", std::get<0>(lineTuple),
               std::get<1>(lineTuple).c_str());
    }
    return 0;
}

int PreProcessor::printOutput() {
    if (error != 0) {
        return error;
    }
    for (auto lineTuple : outLines) {
        printf("%3d:", std::get<0>(lineTuple));
        auto tokens = std::get<1>(lineTuple);
        for (auto token : tokens) {
            printf(" %s", token.c_str());
        }
        printf("\n");
    }
    return 0;
}

int PreProcessor::writeOutput() {
    if (error != 0) {
        return error;
    }
    std::string preName = fileName + ".pre";  // output file name
    
    // Write to pre-processed file
    std::ofstream outFile;
    outFile.open(preName);
    for(auto line : outLines) {
        auto tokens = std::get<1>(line);
        for (auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
            if(std::next(tokenIt) != tokens.end())
            {
                outFile << *tokenIt + " ";
            }
        }
        outFile << tokens.back() + "\n";
    }
    outFile.close();
    
    return 0;
}

std::list<std::tuple<int, std::list<std::string>>> PreProcessor::getOutput() {
    return outLines;
}

int PreProcessor::preProcess() {
    if (error != 0) {
        return error;
    }

    std::map<std::string, int> equMap;
    for (auto lineTupleIt = srcLines.begin(); lineTupleIt != srcLines.end();
         ++lineTupleIt) {
        auto lineTuple = *lineTupleIt;
        int lineCount = std::get<0>(lineTuple);
        std::string line = std::get<1>(lineTuple);

        line = line.substr(0, line.find(";", 0));

        // If line is empty after removing spaces, remove it in pre-processing
        if (!std::all_of(line.begin(), line.end(), isspace)) {
            // Split line in tokens
            auto tokensInLine = tokenize(line);
            // If no token is found, continue on to the next line
            if (tokensInLine.empty()) continue;
            
            auto firstToken = tokensInLine.front();

            // Check if first token is a label
            if (isSuffix(firstToken, ":")) {
                firstToken.pop_back();
                
                // Check if any label used is an already set EQU label
                if (equMap.count(firstToken) > 0) {
                    error = 1;
                    return error;
                }
                
                // Check if label is an EQU label
                auto secondTokenIt = std::next(tokensInLine.begin());
                if (secondTokenIt != tokensInLine.end()){
                    auto secondToken = *std::next(tokensInLine.begin());
                    if (secondToken == "EQU") {
                        auto equLabel = firstToken;
                        auto equValStr = *std::next(tokensInLine.begin(), 2);
                        auto equVal = atoi(equValStr.c_str());
                        auto equTuple = std::make_tuple(equLabel, equVal);
                        equMap[equLabel] = equVal;
                        continue;
                    }
                }
            }

            // Check if any label used is an already set EQU label
            for (auto tokenIt = tokensInLine.begin(); tokenIt != tokensInLine.end(); ++tokenIt) {
                if (equMap.count(*tokenIt) > 0) {
                    *tokenIt = std::to_string(equMap[*tokenIt]);
                }
            }

            // Handle IF labels
            for (auto tokenIt = tokensInLine.begin(); tokenIt != tokensInLine.end(); ++tokenIt) {
                if (*tokenIt == "IF") {
                    auto ifValStr = *std::next(tokenIt);
                    auto ifVal = atoi(ifValStr.c_str());
                    while(tokensInLine.back() != "IF") {
                        tokensInLine.pop_back();
                    }
                    tokensInLine.pop_back();
                    if (ifVal == 0) {
                        ++lineTupleIt;
                    }
                    break;
                }
            }

            if(!tokensInLine.empty()) {
                auto outLine = std::make_tuple(lineCount, tokensInLine);
                outLines.push_back(outLine);
            }
        }
    }
    return 0;
}

int PreProcessor::getError() {
    return error;
}