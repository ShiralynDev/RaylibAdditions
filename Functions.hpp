#include <string>
#include <vector>
#include <raylib.h>

namespace functions {
    // Goes trough string, returns false if it is not of data type integer
    bool stringIsInt(std::string string);
    std::vector<std::string> splitString(std::string string, std::string seperator);
}