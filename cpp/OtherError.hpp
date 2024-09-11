#ifndef OTHER_ERROR_HPP
#define OTHER_ERROR_HPP

#include <cstdio>
#include <string>
#include <vector>

#include "BetterConversion.hpp"

typedef std::string str;
typedef const std::string &cstr;
typedef std::wstring wstr;
typedef const std::wstring &cwstr;
typedef std::vector<wstr> vwstr;
typedef const std::vector<wstr> &cvwstr;

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_GREEN   "\033[32m"

#define ADD_OTHER_ERROR(x) OtherError::addError(x, __PRETTY_FUNCTION__)

class OtherError
{
    static vwstr m_errors;
public:
    static void addError(cwstr errorDesc, cstr from);
    static void printErrors();
    static cvwstr getErrors();
};

#endif // OTHER_ERROR_HPP