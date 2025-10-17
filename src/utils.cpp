/**
 * @author NoxFly
 */

#include <utils.hpp>

std::string ltrim(const std::string& str) {
    size_t start = str.find_first_not_of(" \n\r\t\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string rtrim(const std::string& str) {
    size_t end = str.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string trim(const std::string& str) {
    return rtrim(ltrim(str));
}

std::string replace(const std::string& str, const std::string& find, const std::string& replace) {
    std::string result;
    size_t find_len = find.size();
    size_t pos, from = 0;

    while (std::string::npos != (pos = str.find(find, from))) {
        result.append(str, from, pos - from);
        result.append(replace);
        from = pos + find_len;
    }

    result.append(str, from, std::string::npos);

    return result;
}