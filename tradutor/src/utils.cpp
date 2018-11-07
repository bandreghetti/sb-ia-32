#include <utils.hpp>

using namespace std;

bool fileExists(string filename) {
  ifstream ifile(filename);
  return (bool)ifile;
}

std::list<std::string> tokenize(const std::string s)
{
   std::list<std::string> tokens;
   std::string token;
   std::istringstream tokenStream;
   tokenStream.str(reduce(s));
   while (std::getline(tokenStream, token, ' '))
   {
      for(auto &c : token) {
          c = std::toupper(c);
      }
      tokens.push_back(token);
   }
   return tokens;
}

std::string trim(const std::string& str)
{
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(" \t");
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str)
{
    // trim first
    auto result = trim(str);

    // replace sub ranges
    auto beginSpace = result.find_first_of(" \t");
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(" \t", beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, " ");

        const auto newStart = beginSpace + 1;
        beginSpace = result.find_first_of(" \t", newStart);
    }

    return result;
}

bool isSuffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
