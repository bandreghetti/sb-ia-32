#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

bool fileExists(std::string filename);
std::list<std::string> tokenize(const std::string s);
std::string trim(const std::string &str);
std::string reduce(const std::string &str);
bool isSuffix(const std::string &str, const std::string &suffix);
std::vector<std::string> split(const std::string&, char);
