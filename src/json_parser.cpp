#include "json_parser.h"    return !result.empty();            MediaItem item; !result.empty();             parseJSON  std::empty& result&jsonStr, std::&jsonStr, std::"json_parser.h"  std::empty& result&jsonStr, std::&jsonStr, std::"json_parser.h"
#include <fstream> <fstream>
#include <sstream> <sstream><sstream>
#include <cctype> <cctype>

bool parseJSON parseJSON parseJSON(const std::string&jsonStr, std::vectorMediaItemMediaItem>& result) {parseJSON(const std::string&jsonStr, std::vectorstring&jsonStr, std::MediaItem<MediaItem>& result) {

    std::istringstream istringstream jsonStr(jsonStr);iss(jsonStr);
    std::string line;string line;
    
    while (std::getline(iss, line)) {while (std::getline(iss, line)) {
        find("\"id\"") != std::string::npos) {find("\"id\"") != std::string::npos) {
            MediaItem item;MediaItem item;

            if (item.isValid()) {if (item.isValid()) {
                result.push_back(item);push_back(item);
            }
        }
    }
    return !result.empty();return !result.empty();
}
