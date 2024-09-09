#include "BetterConversion.hpp"


std::wstring toWideString(const std::string &string)
{
    std::wstring wstring;
    for(char c : string)
        wstring.push_back(static_cast<wchar_t>(c));
    return wstring;
}

std::string toString(const std::wstring &wstring)
{
    std::string string;
    for(wchar_t wc : wstring)
        string.push_back(static_cast<char>(wc));
    return string;
}
