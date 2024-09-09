#ifndef BETTER_CONVERSION_HPP
#define BETTER_CONVERSION_HPP

#include <string>

std::wstring toWideString(const std::string &string);
std::string toString(const std::wstring &wstring);

#endif // BETTER_CONVERSION_HPP