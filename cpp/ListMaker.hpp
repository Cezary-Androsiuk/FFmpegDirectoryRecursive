#ifndef LIST_MAKER_HPP
#define LIST_MAKER_HPP

#include <cstdio>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

typedef std::string str;
typedef const str &cstr;
typedef std::vector<str> vstr;
typedef const fs::path &cpath;
typedef std::vector<fs::path> vpath;
typedef std::vector<str> vstr;
typedef const vstr &cvstr;

class ListMaker
{
    static bool vectorContains(cvstr vector, cstr string);
public:
    static vpath listOfFiles(cpath path, cvstr acceptableExtensions, bool printFoundFiles = false);
};


#endif