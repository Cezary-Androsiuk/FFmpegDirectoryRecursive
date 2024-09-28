#include <cstdio>
#include <string>
#include <vector>
#include <filesystem>

#include "cpp/OtherError.hpp"
#include "cpp/MainMethods.hpp"
#include "cpp/FFExecute.hpp"
#include "cpp/ListMaker.hpp"
#include "cpp/enums/SkipAction.hpp"
#include "cpp/WinConsoleHandler.hpp"

// locate files recursive

// test commands:
// xcopy .\tests\backupenv\test\ .\tests\test\ /E /H /Q
// ffmpegRec .\tests\test avi+mkv+mp4+ move
// rd /S /Q .\tests\test
// rd /S /Q .\tests\test-ffmpeg-h.265
// rd /S /Q .\tests\test-finished_source_files

// prepare test env
// rd /S /Q .\tests\test && rd /S /Q .\tests\test-ffmpeg-h.265 && rd /S /Q .\tests\test-finished_source_files && xcopy .\tests\backupenv\test\ .\tests\test\ /E /H /Q


// assuming that ffmpeg is already installed
// instalation(add ffmpegRec.exe to PATH environment):
// 1 in windows search type "Edit the system environment variables" 
// 2 press "Environment Variables..." button
// 3 in bottom part (System variables) find variable named "Path" and double click on it
// 4 press on the right site the "New" button and type path to directory, where executable file (created after compilation) is located
// in example path to this executable (for now) is "D:\vscode\c++\projects\FFmpegDirectoryRecursive(github)"
// now, you can open cmd in any directory and just in command prompt type "ffmpegRec . mkv+mp4 move"

#define IN_DEBUG false

#define VERSION "1.3.14"
const char *versionLine = "FFmpegRec version " VERSION "\n";

int main(int argc, const char **argv)
{
    printf("\n%s", versionLine);
    HandlePipeOutput::setVersionToSave(versionLine);

    fs::path inDirectory;
    vstr extensions;
    SkipAction skipAction;
    void* arguments[] = {&inDirectory, &extensions, &skipAction};
    if( !handleArgs(argc, argv, arguments) ) // messages are handle in 'handleArgs' function
        return 1;

    if(!WinConsoleHandler::installConsoleHandler()) // messages are handle in 'installConsoleHandler' method
        return 1;


    printf("Selected directory: %ls\n", inDirectory.wstring().c_str());
    HandlePipeOutput::setFFOFileDirectory(inDirectory);

    // create list of files
    vpath listOfFiles = ListMaker::listOfFiles(inDirectory, extensions); // listOfFiles method also prints files
    
    fs::path outDirectory, OFCDirectory;
    if(skipAction != SkipAction::Test)
    {
        outDirectory = createOutputDirectory(inDirectory, IN_DEBUG);
        if(outDirectory == fs::path()) // messages are handle in 'createOutputDirectory' function
            return 1;

        OFCDirectory = createOCFDirectory(inDirectory, IN_DEBUG);
        if(OFCDirectory == fs::path()) // messages are handle in 'createOCFDirectory' function
            return 1;
    }

    FFExecute::setTotalFFmpegsToPerform(listOfFiles.size());
    FFExecute::setSkipAction(skipAction);
    
    printStatusInfo(skipAction);
    if(skipAction != SkipAction::Test)
    {
        str filesProgress = FFExecute::makeFileProgressPostfix();
        printf("Status: %s\n\n", filesProgress.c_str()); 
    }
    bool loopBroken = false;
    for(const auto &inFile : listOfFiles)
    {
        // all files in list are valid at this point
        fs::path outFile = createOutputFile(inFile, inDirectory, outDirectory);
        fs::path OFCFile = createOFCFile(inFile, inDirectory, OFCDirectory);
        
        FFExecute::runFFmpeg(inFile, outFile, OFCFile);
        if(WinConsoleHandler::combinationCtrlCPressed())
        {
            printf("files loop terminated due to Ctrl+C was pressed\n");
            loopBroken = true;
            break;
        }
        
        // handle case, when when the drive disconnect or something
        if(fs::exists(inDirectory));
            continue;

        printf(COLOR_RED "inDirectory no longer exist" COLOR_RESET "!\n");
        loopBroken = true;
        break;
    }

    deleteDirectoryIfEmpty(outDirectory);
    deleteDirectoryIfEmpty(OFCDirectory);

    if(!loopBroken)
    {
        int correctlyPerformedFFmpegs = FFExecute::getCorrectlyPerformedFFmpegs();
        double correctlyPerformedRatio = 
            static_cast<double>(correctlyPerformedFFmpegs) / static_cast<double>(listOfFiles.size());

        if(correctlyPerformedRatio == 1.0)
            printf(COLOR_GREEN "Finished all FFmpegs" COLOR_RESET "!\n");
        else
            printf(COLOR_WHITE "Finished " COLOR_RED "%g%%" COLOR_WHITE " of FFmpegs" COLOR_RESET "!\n", correctlyPerformedRatio * 100);
    }
    else
    {
        printf(COLOR_RED "Program was stopped" COLOR_RESET "!\n");
    }

    OtherError::printErrors();
}