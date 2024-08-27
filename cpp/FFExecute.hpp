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
#include "enums/SkipAction.hpp"

namespace fs = std::filesystem;

typedef std::string str;
typedef const std::string &cstr;

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
    static str changeOutputFileNameIfNeeded(cstr fileName);

    static void openFFOFile(); // FFOFile is FFmpegOutputFile
    static void addTextToFFOFile(cstr ffmpegOutput); // FFOFile is FFmpegOutputFile
    static void closeFFOFile(); // FFOFile is FFmpegOutputFile

    static int lengthOfNumber(int number);
    static str numberThatOccupiesGivenSpace(int number, int space);

    static void clearLine(int len);
    static str splitNumberByThousands(int number, char separator = ' ');
    static void printProgress(int progress);

    static void skipFileAction();
    static bool copyFileAction(cstr from, cstr to);
    static bool moveFileAction(cstr from, cstr to);

    static void moveCorrectlyFinishedFile(cstr from, cstr to);

    static void handleAlreadyH265File(cstr inFile, str outFile);
    
    static void runFFmpegTest(cstr inFile);
    static void runFFmpegForce(cstr inFile, cstr outFile, cstr moveFile);
    static void runFFmpegStandard(cstr inFile, cstr outFile, cstr moveFile);
    
    static void runFFmpegTest2(cstr inFile);
    static void runFFmpegForce2(cstr inFile, str outFile, cstr moveFile);
    static void runFFmpegStandard2(cstr inFile, str outFile, cstr moveFile);

public:
    static str makeFileProgressPostfix(bool addColors = true);
    static void setTotalFFmpegsToPerform(int count);
    static void setSkipAction(SkipAction skipAction);
    static void setffOFileDirectory(fs::path directory);
    static void runFFmpeg(cstr inFile, cstr outFile, cstr moveFile);

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