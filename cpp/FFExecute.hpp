#ifndef FFEXECUTE_HPP
#define FFEXECUTE_HPP

#include <cstdio>
#include <string>
#include <sstream> // for getInterpretationOfTime
#include <ctime>
#include <fstream>
#include <filesystem>

#include "FFTester.hpp"
#include "OtherError.hpp"
#include "ChangeFileDate.hpp"
#include "enums/SkipAction.hpp"

namespace fs = std::filesystem;

typedef std::string str;
typedef const std::string &cstr;
typedef const fs::path &cpath;

#define FFMPEG_OUTPUT_FILE(time) ("FFmpeg_output-" + time + ".txt") 

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define TEXT_BOLD     "\033[1m"


// ,,Since a Microsoft compiler is used, an underscore is needed at the beginning''
// https://stackoverflow.com/questions/38876218/execute-a-command-and-get-output-popen-and-pclose-undefined
#define pipeOpen(...) _popen(__VA_ARGS__)
#define pipeClose(...) _pclose(__VA_ARGS__)

class FFExecute
{
    static void handleOutput(cstr line);
    static void printOutputToCMD(cstr line);
    static long long myStoll(cstr string) noexcept;
    static size_t getInterpretationOfTime(cstr strtime);
    static str getCurrentTime();
    static fs::path changeOutputFileNameIfNeeded(fs::path path);

    static void openFFOFile(); // FFOFile is FFmpegOutputFile
    static void addTextToFFOFile(cstr ffmpegOutput); // FFOFile is FFmpegOutputFile
    static void closeFFOFile(); // FFOFile is FFmpegOutputFile

    static int lengthOfNumber(int number);
    static str numberThatOccupiesGivenSpace(int number, int space);

    static void clearLine(int len);
    static str splitNumberByThousands(int number, char separator = ' ');
    static void printProgress(int progress);

    static void skipFileAction();
    static bool copyFileAction(cpath from, cpath to);
    static bool moveFileAction(cpath from, cpath to);

    static void moveDateOfFile(cpath from, cpath to);
    static void moveCorrectlyFinishedFile(cpath from, cpath to);

    static void handleAlreadyH265File(cpath inFile, cpath outFile);
    static str makeStringPathExistForCMD(cpath path);
    
    static void runFFmpegTest(cpath inFile);
    static void runFFmpegForce(cpath inFile, cpath outFile, cpath moveFile);
    static void runFFmpegStandard(cpath inFile, cpath outFile, cpath moveFile);
    
    static void runFFmpegTest2(cpath inFile);
    static void runFFmpegForce2(cpath inFile, cpath outFile, cpath moveFile);
    static void runFFmpegStandard2(cpath inFile, cpath outFile, cpath moveFile);

public:
    static str makeFileProgressPostfix(bool addColors = true);
    static void setTotalFFmpegsToPerform(int count);
    static void setSkipAction(SkipAction skipAction);
    static void setffOFileDirectory(fs::path directory);
    static void runFFmpeg(cpath inFile, cpath outFile, cpath moveFile);

private:
    static int m_performedFFmpegs;
    static int m_correctlyPerformedFFmpegs;
    static int m_failedFFmpegs;
    static int m_skippedFFmpegs;
    static int m_totalFFmpegsToPerform;
    static SkipAction m_skipAction;
    
    static std::ofstream m_ffOFile; // ffOFile is FFmpegOutputFile
    static str m_ffOFileName; // ffOFile is FFmpegOutputFile
    static fs::path m_ffOFileDirectory;
    static str m_strDuration;
};

#endif