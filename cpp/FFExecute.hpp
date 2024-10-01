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
#include "BetterConversion.hpp"
// #include "TemporaryRename.hpp"

namespace fs = std::filesystem;

typedef std::string str;
typedef const std::string &cstr;
typedef const fs::path &cpath;

#define TRY(x, y) try{ x } catch(std::exception &e) {printf("%s %s\n", std::string(y).c_str(), e.what());}

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

#define _CMDT(text) L" \"" + (text) + L"\" " /* command text */
#define FFMPEG_COMMAND(input, output) \
    L"ffmpeg -i" _CMDT(input) L"-c:v libx265 -vtag hvc1" _CMDT(output) \
    L"2>&1"; // move stderr to stdout (connect them)

class FFExecute
{
    static fs::path changeOutputFileNameIfNeeded(fs::path path);

    static void skipFileAction();
    static bool copyFileAction(cpath from, cpath to);
    static bool moveFileAction(cpath from, cpath to);
    static void handleAlreadyH265File(cpath inFile, cpath outFile);

    static void handleStop(cpath inFile, cpath outFile);
    static void moveDateOfFile(cpath from, cpath to);
    static void moveCorrectlyFinishedFile(cpath from, cpath to);
    
    static void runFFmpegTest(cpath inFile);
    static void runFFmpegStandard(cpath inFile, fs::path outFile, cpath moveFile);
    
    static bool _existCase(cpath inFile);
    static bool _testPipePart(cpath inFile);
    static bool _ffprobePartForTest(cpath inFile);
    static bool _ffprobePartForStandard(cpath inFile, cpath outFile);
    static bool _ffmpegPartForStandard(cpath inFile, cpath outFile);


public:
    static str makeFileProgressPostfix(bool addColors = true);
    static void setTotalFFmpegsToPerform(int count);
    static void setSkipAction(SkipAction skipAction);
    static void runFFmpeg(cpath inFile, cpath outFile, cpath moveFile);
    static int getLastExecuteStatus();
    static int getCorrectlyPerformedFFmpegs();

private:
    static int m_performedFFmpegs;
    static int m_correctlyPerformedFFmpegs;
    static int m_failedFFmpegs;
    static int m_skippedFFmpegs;
    static int m_totalFFmpegsToPerform;
    static SkipAction m_skipAction;
    static int m_lastExecuteStatus;
};

#endif