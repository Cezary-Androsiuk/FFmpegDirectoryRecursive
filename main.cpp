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

// compile:
// g++ main.cpp cpp\MainMethods.cpp cpp\FFExecute.cpp cpp\FFTester.cpp cpp\ListMaker.cpp cpp\OtherError.cpp -o ffmpegRec.exe

// instalation(add ffmpegRec.exe to PATH environment):
// 1 in windows search type "Edit the system environment variables" 
// 2 press "Environment Variables..." button
// 3 in bottom part (System variables) find variable named "Path" and double click on it
// 4 press on the right site the "New" button and type path to directory, where executable file (created after compilation) is located
// in example path to this executable (for now) is "D:\vscode\c++\projects\FFmpegDirectoryRecursive(github)"
// now, you can open cmd in any directory and just in command prompt type "ffmpegRec . mkv+mp4 move"

#define IN_DEBUG false

int main(int argc, const char **argv)
{
    printf("\n");

    fs::path inDirectory;
    vstr extensions;
    SkipAction skipAction;
    void* arguments[] = {&inDirectory, &extensions, &skipAction};
    if( !handleArgs(argc, argv, arguments) ) // messages are handle in 'handleArgs' function
        return 1;

    if(!WinConsoleHandler::installConsoleHandler()) // messages are handle in 'installConsoleHandler' method
        return 1;


    printf("Selected directory: %ls\n", inDirectory.wstring().c_str());
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


    printf("Found files:\n");
    vpath listOfFiles = ListMaker::listOfFiles(inDirectory, extensions); // listOfFiles method also prints files

    FFExecute::setTotalFFmpegsToPerform(listOfFiles.size());
    FFExecute::setSkipAction(skipAction);
    HandlePipeOutput::setFFOFileDirectory(inDirectory);
    
    printStatusInfo(skipAction);
    if(skipAction != SkipAction::Test)
    {
        str filesProgress = FFExecute::makeFileProgressPostfix();
        printf("Status: %s\n\n", filesProgress.c_str()); 
    }
    for(const auto &inFile : listOfFiles)
    {
        // all files in list are valid at this point
        fs::path outFile = createOutputFile(inFile, inDirectory, outDirectory);
        fs::path OFCFile = createOFCFile(inFile, inDirectory, OFCDirectory);
        
        FFExecute::runFFmpeg(inFile, outFile, OFCFile);
        if(WinConsoleHandler::combinationCtrlCPressed())
        {
            printf("files loop terminated due to Ctrl+C was pressed\n");
            break;
        }
        
        // handle case, when when the drive disconnect or something
        // but first check if failed
        if(FFExecute::getLastExecuteStatus() != 0)
            continue;

        if(!fs::exists(inDirectory));
            continue;

        printf("inDirectory no longer exist!\n");
        break;
    }

    deleteDirectoryIfEmpty(outDirectory);
    deleteDirectoryIfEmpty(OFCDirectory);

    printf("Finished all FFmpegs!\n");

    OtherError::printErrors();
}