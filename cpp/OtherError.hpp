#ifndef OTHER_ERROR_HPP
#define OTHER_ERROR_HPP

#include <cstdio>
#include <string>
#include <vector>

typedef std::string str;
typedef const std::string &cstr;
typedef std::vector<str> vstr;
typedef const std::vector<std::string> &cvstr;

class OtherError
{
    static vstr m_errors;
public:
    static void addError(cstr errorDesc, cstr from);
    static void printErrors();
    static cvstr getErrors();
};

#endif // OTHER_ERROR_HPP