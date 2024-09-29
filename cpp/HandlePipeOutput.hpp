#ifndef HANDLE_PIPE_OUTPUT_HPP
#define HANDLE_PIPE_OUTPUT_HPP

#include <cstdio>
#include <string>
#include <sstream> // for getInterpretationOfTime
#include <fstream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <random>
#include <queue> // for splitSpamLine
#include <cctype> // for cleanFFOFile

#include "OtherError.hpp"
#include "BetterConversion.hpp"

namespace fs = std::filesystem;

typedef std::string str;
typedef std::wstring wstr;
typedef const std::string &cstr;
typedef const std::wstring &cwstr;
typedef std::vector<std::wstring> vwstr;
typedef const fs::path &cpath;

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"

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
    static void addTextToFFOFile(cstr line);



    static void printProgress(int progress, cstr duration);
    static void handleOutput(cstr line);
    static void addToFFOFile(cstr text);

    static void openFFOFile();
    static void closeFFOFile();

    static fs::path moveFFOFileToTemporary();
    static bool lineIsSpam(cwstr line);
    static void splitSpamLine(wstr line, std::queue<wstr> &first, std::queue<wstr> &last, const int &linesCount);
    static wstr makeSpamShorter(cwstr line);
    static void cleanFFOFile();

    // setters / getters
    static void setFFOFileDirectory(cpath ffOFileDirectory);
    static void setStringDuration(cstr stringDuration);
    static void setVersionToSave(const char * versionToSave);

private:
    static bool m_ffOFileIsOpen;
    static bool m_lastFFOFileWriteFailed;
    static std::string m_stringDuration;
    static std::ofstream m_ffOFile; // ffOFile is FFmpegOutputFile
    static fs::path m_ffOFileDirectory; // ffOFile is FFmpegOutputFile
    static fs::path m_ffOFilePath; // ffOFile is FFmpegOutputFile
    static const char *m_versionToSave;
};



#endif