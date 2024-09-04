#ifndef FFEXECUTE_HPP
#define FFEXECUTE_HPP

#include <cstdio>
#include <string>
#include <ctime>
#include <fstream>
#include <filesystem>

#include "FFTester.hpp"
#include "OtherError.hpp"
#include "ChangeFileDate.hpp"
#include "TestPipe.hpp"
#include "enums/SkipAction.hpp"
#include "WinConsoleHandler.hpp"
#include "HandlePipeOutput.hpp"

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
#define wpipeOpen(...) _wpopen(__VA_ARGS__)
#define pipeClose(...) _pclose(__VA_ARGS__)

class FFExecute
{
    static fs::path changeOutputFileNameIfNeeded(fs::path path);

    static void skipFileAction();
    static bool copyFileAction(cpath from, cpath to);
    static bool moveFileAction(cpath from, cpath to);
    static void handleAlreadyH265File(cpath inFile, cpath outFile);

    static void moveDateOfFile(cpath from, cpath to);
    static void moveCorrectlyFinishedFile(cpath from, cpath to);
    
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
    static void runFFmpeg(cpath inFile, cpath outFile, cpath moveFile);

private:
    static int m_performedFFmpegs;
    static int m_correctlyPerformedFFmpegs;
    static int m_failedFFmpegs;
    static int m_skippedFFmpegs;
    static int m_totalFFmpegsToPerform;
    static SkipAction m_skipAction;
    
};

#endif