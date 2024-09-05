#include "ChangeFileDate.hpp"

void (*ChangeFileDate::m_addTextToFFOFile)(cstr) = nullptr;

void ChangeFileDate::ownFromFileTimeToSystemTime(
    const FILETIME * const fileTime, 
    SYSTEMTIME * const systemTime)
{
    SYSTEMTIME utcSystemTime;
    FileTimeToSystemTime(fileTime, &utcSystemTime);
    SystemTimeToTzSpecificLocalTime(NULL, &utcSystemTime, systemTime);
}

void ChangeFileDate::ownFromSystemTimeToFileTime(
    const SYSTEMTIME * const systemTime, 
    FILETIME * const fileTime)
{
    SYSTEMTIME localSystemTime;
    TzSpecificLocalTimeToSystemTime(NULL, systemTime, &localSystemTime);
    SystemTimeToFileTime(&localSystemTime, fileTime);
}
    

bool ChangeFileDate::getFileTime(
    const char * const filename, 
    SYSTEMTIME * const creationSystemTime, 
    SYSTEMTIME * const modificationSystemTime)
{
    HANDLE hfile = CreateFileA(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(hfile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "    Error while oppening file %s\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while oppening file %s\n") + filename);
        return false;
    }

    FILETIME creationFileTime, modificationFileTime;

    if(!GetFileTime(hfile, &creationFileTime, NULL, &modificationFileTime))
    {
        fprintf(stderr, "    Error while reading creation and modification time from file %s\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while reading creation and modification time from file %s\n") + filename);
        CloseHandle(hfile);
        return false;
    }

    ChangeFileDate::ownFromFileTimeToSystemTime(&creationFileTime, creationSystemTime);
    ChangeFileDate::ownFromFileTimeToSystemTime(&modificationFileTime, modificationSystemTime);

    CloseHandle(hfile);

    return true;
}

bool ChangeFileDate::setFileTime(
    const char * const filename, 
    const SYSTEMTIME * const creationSystemTime, 
    const SYSTEMTIME * const modificationSystemTime)
{
    HANDLE hfile = CreateFileA(
        filename,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(hfile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "    Error while oppening file %s\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while oppening file %s\n") + filename);
        return false;
    }

    FILETIME creationFileTime, modificationFileTime;
    ChangeFileDate::ownFromSystemTimeToFileTime(creationSystemTime, &creationFileTime);
    ChangeFileDate::ownFromSystemTimeToFileTime(modificationSystemTime, &modificationFileTime);

    if(!SetFileTime(hfile, &creationFileTime, NULL, &modificationFileTime))
    {
        fprintf(stderr, "    Error while saving creation and modification time to file %s\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while saving creation and modification time to file %s\n") + filename);
        CloseHandle(hfile);
        return false;
    }

    CloseHandle(hfile);

    return true;
}

str ChangeFileDate::stringTimeFromSystemTime(const SYSTEMTIME * const systemTime)
{
    char buffer[128];
    sprintf(buffer, "%04d-%02d-%02d_%02d-%02d-%02d", 
        systemTime->wYear, systemTime->wMonth, systemTime->wDay,
        systemTime->wHour, systemTime->wMinute, systemTime->wSecond);
    return std::string(buffer);
}

void ChangeFileDate::addTextToFFOFile(cstr text)
{
    if(m_addTextToFFOFile != nullptr)
        m_addTextToFFOFile(text);
    else
        printf("    m_addTextToFFOFile in ChangeFileDate was not specyfied, text: %s\n", text.c_str());
}

str ChangeFileDate::makeStringPathExistForCMD(cstr path)
{
    fs::path p(path);
    std::wstring ws(p.wstring());
    return str(ws.begin(), ws.end());
}

bool ChangeFileDate::fromFileToFile(cstr from, cstr to)
{
    if( !fs::exists(from) || !fs::exists(to) )
    {
        fprintf(stderr, "    one of the files not exist can't change file date '%s' '%s' !\n", 
            from.c_str(), to.c_str());
        ChangeFileDate::addTextToFFOFile("one of the files not exist can't change file date '" + 
            from + "' '" + to);
        return false;
    }
    
    SYSTEMTIME creationTime, modificationTime;
    
    if(!getFileTime(ChangeFileDate::makeStringPathExistForCMD(from).c_str(), &creationTime, &modificationTime))
    {
        fprintf(stderr, "    getFileTime method failed!\n");
        ChangeFileDate::addTextToFFOFile("getFileTime method failed!\n");
        return false;
    }

    str date = "{ creationTime: " + ChangeFileDate::stringTimeFromSystemTime(&creationTime) + 
        ", modificationTime: " + ChangeFileDate::stringTimeFromSystemTime(&modificationTime) + " }";

    ChangeFileDate::addTextToFFOFile("from file '" + from + "' readed date " + date);

    if(!setFileTime(ChangeFileDate::makeStringPathExistForCMD(to).c_str(), &creationTime, &modificationTime))
    {
        fprintf(stderr, "    setFileTime method failed!\n");
        ChangeFileDate::addTextToFFOFile("setFileTime method failed!\n");
        return false;
    }

    ChangeFileDate::addTextToFFOFile("date " + date + " assigned to file '" + to + "'");

    return true;
}

void ChangeFileDate::setHandleFFprobeOutput(void (*func)(cstr))
{
    m_addTextToFFOFile = func;
}