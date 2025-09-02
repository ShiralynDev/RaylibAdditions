#include "Functions.hpp"

bool functions::stringIsInt(std::string string) {
    if (string.length() == 0) 
        return false;
    for (int i = 0; i < string.length(); i++) {
        if (isdigit(string[i]) == false) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> functions::splitString(std::string string, std::string seperator) {
    std::vector<std::string> returnString;
    std::string modString = string;
    bool seperatorLeft = true;
    while (seperatorLeft) {
        if (modString.find(seperator) == std::string::npos) {
            seperatorLeft = false;
            returnString.push_back(modString);
            break;
        }
        returnString.push_back(modString.substr(0, modString.find(seperator)));
        modString = modString.substr(modString.find(seperator) + seperator.size(), modString.length());
    }
    return returnString;
}