#ifndef CHANGE_FILE_DATE_HPP
#define CHANGE_FILE_DATE_HPP

#include <cstdio>
#include <string>
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

typedef std::string str;
typedef const str &cstr;


class ChangeFileDate
{
    static void ownFromFileTimeToSystemTime(
        const FILETIME * const fileTime, SYSTEMTIME * const systemTime);

    static void ownFromSystemTimeToFileTime(
        const SYSTEMTIME * const systemTime, FILETIME * const fileTime);


    static bool getFileTime(const char * const filename, 
        SYSTEMTIME * const creationSystemTime, 
        SYSTEMTIME * const modificationSystemTime);

    static bool setFileTime(const char * const filename, 
        const SYSTEMTIME * const creationSystemTime, 
        const SYSTEMTIME * const modificationSystemTime);

    static str stringTimeFromSystemTime(const SYSTEMTIME * const);
    static void addTextToFFOFile(cstr text);
    static str makeStringPathExistForCMD(cstr path);

public:
    static bool fromFileToFile(cstr from, cstr to);
    static void setHandleFFprobeOutput(void (*func)(cstr));

private:
    static void (*m_addTextToFFOFile)(cstr);
};

#endif // CHANGE_FILE_DATE_HPP