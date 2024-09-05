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
    const wchar_t * const filename, 
    SYSTEMTIME * const creationSystemTime, 
    SYSTEMTIME * const modificationSystemTime)
{
    HANDLE hfile = CreateFileW(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    str strFilename(std::wstring(filename).begin(), std::wstring(filename).end());

    if(hfile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "    Error while oppening file %ls\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while oppening file %s\n") + strFilename);
        return false;
    }

    FILETIME creationFileTime, modificationFileTime;

    if(!GetFileTime(hfile, &creationFileTime, NULL, &modificationFileTime))
    {
        fprintf(stderr, "    Error while reading creation and modification time from file %ls\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while reading creation and modification time from file: ") + 
            strFilename + "\n");
        CloseHandle(hfile);
        return false;
    }

    ChangeFileDate::ownFromFileTimeToSystemTime(&creationFileTime, creationSystemTime);
    ChangeFileDate::ownFromFileTimeToSystemTime(&modificationFileTime, modificationSystemTime);

    CloseHandle(hfile);

    return true;
}

bool ChangeFileDate::setFileTime(
    const wchar_t * const filename, 
    const SYSTEMTIME * const creationSystemTime, 
    const SYSTEMTIME * const modificationSystemTime)
{
    HANDLE hfile = CreateFileW(
        filename,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    str strFilename(std::wstring(filename).begin(), std::wstring(filename).end());

    if(hfile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "    Error while oppening file %ls\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while oppening file %s\n") + strFilename);
        return false;
    }

    FILETIME creationFileTime, modificationFileTime;
    ChangeFileDate::ownFromSystemTimeToFileTime(creationSystemTime, &creationFileTime);
    ChangeFileDate::ownFromSystemTimeToFileTime(modificationSystemTime, &modificationFileTime);

    if(!SetFileTime(hfile, &creationFileTime, NULL, &modificationFileTime))
    {
        fprintf(stderr, "    Error while saving creation and modification time to file %ls\n", filename);
        ChangeFileDate::addTextToFFOFile(str("Error while saving creation and modification time to file: ") + 
            strFilename + "\n");
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

bool ChangeFileDate::fromFileToFile(cpath from, cpath to)
{
    const str strFrom(from.wstring().begin(), from.wstring().end());
    const str strTo(to.wstring().begin(), to.wstring().end());
    if( !fs::exists(from) || !fs::exists(to) )
    {
        fprintf(stderr, "    one of the files not exist can't change file date '%s' '%s' !\n", strFrom.c_str(), strTo.c_str());
        ChangeFileDate::addTextToFFOFile("one of the files not exist can't change file date '" + strFrom + "' '" + strTo);
        return false;
    }
    
    SYSTEMTIME creationTime, modificationTime;
    
    if(!ChangeFileDate::getFileTime(from.wstring().c_str(), &creationTime, &modificationTime))
    {
        fprintf(stderr, "    getFileTime method failed!\n");
        ChangeFileDate::addTextToFFOFile("getFileTime method failed!\n");
        return false;
    }

    str date = "{ creationTime: " + ChangeFileDate::stringTimeFromSystemTime(&creationTime) + 
        ", modificationTime: " + ChangeFileDate::stringTimeFromSystemTime(&modificationTime) + " }";

    ChangeFileDate::addTextToFFOFile("from file '" + strFrom + "' readed date " + date);

    if(!ChangeFileDate::setFileTime(to.wstring().c_str(), &creationTime, &modificationTime))
    {
        fprintf(stderr, "    setFileTime method failed!\n");
        ChangeFileDate::addTextToFFOFile("setFileTime method failed!\n");
        return false;
    }

    ChangeFileDate::addTextToFFOFile("date " + date + " assigned to file '" + strTo + "'");

    return true;
}

void ChangeFileDate::setHandleFFprobeOutput(void (*func)(cstr))
{
    m_addTextToFFOFile = func;
}