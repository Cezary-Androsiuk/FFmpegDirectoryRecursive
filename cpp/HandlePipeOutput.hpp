#ifndef HANDLE_PIPE_OUTPUT_HPP
#define HANDLE_PIPE_OUTPUT_HPP

#include <cstdio>
#include <string>
#include <sstream> // for getInterpretationOfTime
#include <fstream>
#include <filesystem>
#include <queue> // for cleanFFOFile

namespace fs = std::filesystem;

typedef std::string str;
typedef std::wstring wstr;
typedef const std::string &cstr;
typedef const std::wstring &cwstr;
typedef const fs::path &cpath;

class HandlePipeOutput
{
public:
    static long long myStoll(cstr string) noexcept;
    static size_t getInterpretationOfTime(cstr stringTime);
    static void clearLine(int len);
    static str splitNumberByThousands(int number, char separator);
    static str numberThatOccupiesGivenSpace(int number, int space);
    static str getCurrentTime();
    static void createFFOFilePath();

    static void printOutputToCMD(cstr line);
    static void addTextToFFOFile(cwstr line);



    static void printProgress(int progress, cstr duration);
    static void handleOutput(cstr line);
    static void addToFFOFile(cstr text);
    static void addToFFOFile(cwstr text);

    static void openFFOFile();
    static void closeFFOFile();

    static fs::path moveFFOFileToTemporary();
    static bool lineIsSpam(cwstr line);
    static void cleanFFOFile();

    // setters / getters
    static void setFFOFileDirectory(cpath ffOFileDirectory);
    static void setStringDuration(cstr stringDuration);

private:
    static bool m_ffOFileIsOpen;
    static std::string m_stringDuration;
    static std::wofstream m_ffOFile; // ffOFile is FFmpegOutputFile
    static fs::path m_ffOFileDirectory; // ffOFile is FFmpegOutputFile
    static fs::path m_ffOFilePath; // ffOFile is FFmpegOutputFile
};



#endif