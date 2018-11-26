#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <utils.hpp>
#include <preprocessor.hpp>
#include <translator.hpp>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Missing arguments! Expecting 1:" << endl
        << "Usage: montador <file-to-assemble-without-extension>" << endl;
        return -1;
    }

    string fileName = string(argv[1]);

    PreProcessor pp(fileName);
    if(pp.getError()) {
        return -1;
    }
    auto err = pp.preProcess();
    if (err) {
        cout << "Something wrong happened during pre-processing\n";
        return -1;
    }

    Translator translator(fileName, pp.getOutput());

    err = translator.translate();
    if (err) {
        cout << "error during translation: " + translator.getErrorMessage() << std::endl;
        return -1;
    }

    err = translator.writeOutput();
    if (err) {
        cout << "error during writing: " + translator.getErrorMessage() << std::endl;
        return -1;
    }

    return 0;
}
